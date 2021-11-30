/*
 * Copyright 2002-2021 oZ/acy (名賀月晃嗣)
 * Redistribution and use in source and binary forms, 
 *     with or without modification, 
 *   are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * @file dibin.cpp
 * @author by oZ/acy (名賀月晃嗣)
 * @brief DIB形式畫像ファイルの讀み込み
 *
 * @date 2016.3.2 C++11對應
 * @date 2018.12.23 C++17對應
 * @date 2018.12.28 資源管理をunique_ptrに移行
 * @date 2019.8.29 new[]をmake_uniqueに置換
 * @date 2021.4.29 LIBPOLYMNIAからLIBEUNOMIAに移植
 *
 */
#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <memory>
#include "dibio.h"


namespace
{
/*
 * @brief DIBファイル情報(の一部)
 */
struct Info_
{
  int width;  // 幅
  int height; // 高さ
  int bpp;    // Bit per Pixel
  int numpalette;  // パレット数
};


//// ファイル先頭部の讀み込み處理 ////////

/*
 * @brief DIBヘッダの讀み込み 
 */
bool readHeader_(std::istream& is)
{
  std::uint16_t w;
  std::uint32_t d;

  // マジックナンバー("BM")の讀み込みと照合
  if (!is.read((char*)&w, sizeof(std::uint16_t)))
    return false;
  if (w != *(std::uint16_t*)("BM"))
    return false;

  // ファイルサイズ讀み込み(讀み捨て)
  if (!is.read((char*)&d, sizeof(std::uint32_t)))
    return false;

  // 予約済ワード(2つ)讀み込み(讀み捨て)
  if (!is.read((char*)&w, sizeof(std::uint16_t)))
    return false;
  if (!is.read((char*)&w, sizeof(std::uint16_t)))
    return false;

  // BitDataへのオフセット讀み込み(讀み捨て)
  if (!is.read((char*)&d, sizeof(std::uint32_t)))
    return false;

  return true;
}

/*
 * @brief BMP-Infoヘッダの讀み込み
 *
 * BMP-Infoヘッダを讀み込んで、Info_に讀み込んだ値を設定する。
 * @param is 入力ストリーム
 * @param info 讀み込んだ情報を設定するInfo_オブジェクト
 * @return 讀み込みに成功した場合はtrue、さもなくばfalseを返す。
 */
bool readInfo_(std::istream& is, Info_& info)
{
  std::uint16_t w;
  std::uint32_t u;
  std::int32_t d;

  // InfoHeader のサイズ
  if (!is.read((char*)&u, sizeof(std::uint32_t)))
    return false;

  // 畫像幅
  if (!is.read((char*)&d, sizeof(std::int32_t)))
    return false;
  info.width = d;

  // 畫像高さ
  if (!is.read((char*)&d, sizeof(std::int32_t)))
    return false;
  info.height = d;


  if (!is.read((char*)&w, sizeof(std::uint16_t)))
    return false;

  // bits per pixel
  if (!is.read((char*)&w, sizeof(std::uint16_t)))
    return false;
  info.bpp = w;

  if (!is.read((char*)&u, sizeof(std::uint32_t)))
    return false;
  if (!is.read((char*)&u, sizeof(std::uint32_t)))
    return false;
  if (!is.read((char*)&d, sizeof(std::int32_t)))
    return false;
  if (!is.read((char*)&d, sizeof(std::int32_t)))
    return false;

  // パレット數
  if (!is.read((char*)&u, sizeof(std::uint32_t)))
    return false;
  info.numpalette = u;

  if (!is.read((char*)&u, sizeof(std::uint32_t)))
    return false;

  return true;
}


/*
 * @brief パレットの讀み込み
 */
bool readPalette_(std::istream& is, eunomia::RgbColour pal[], int npal)
{
  struct
  {
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t v;
  } quad[256];

  if (!is.read((char*)quad, sizeof(std::uint8_t) * 4 * npal))
    return false;

  for (int i = 0; i < npal; ++i) {
    pal[i].red = quad[i].r;
    pal[i].green = quad[i].g;
    pal[i].blue = quad[i].b;
  }

  return true;
}


//// 補助函數 ////////

/*
 * @brief Bitmapの1lineのバッファ長の算出
 */
inline constexpr int getBufSize_(int l) noexcept
  { return (l + 3) & ~3; }


/*
 * @brief 1ライン分の1bppの畫素データを1byte/pixelに展開する
 */
void
read01_oneline_(std::uint8_t src[], int width, std::uint8_t dst[])
{
  for (int i = 0; i < width / 8; ++i)
    for (int j = 7; j >= 0; --j) {
      dst[i * 8 + j] = src[i] & 1;
      src[i] >>= 1;
    }

  int dw = width & 7;
  int uw = width & ~7;
  for (int i = dw - 1; i >= 0; --i) {
    dst[uw + i] = src[uw >> 3] & 1;
    src[uw >> 3] >>= 1;
  }
}


/*
 * @brief 1ライン分の4bppの畫素データを1byte/pixelに展開する
 */
void
read04_oneline_(std::uint8_t src[], int width, std::uint8_t dst[])
{
  for (int i = 0; i < width / 2; i++) {
    dst[i * 2 + 1] = src[i] & 0x0F;
    dst[i * 2] = (src[i] >> 4) & 0x0F;
  }

  if (width & 1)
    dst[width - 1] = src[width / 2] & 0x0F;
}




//// 實際の讀み込み處理 ////////

/*
 * @brief 24bit/畫素のビットマップの讀み込み
 */
bool read24bit_(std::istream& is, eunomia::Picture& pict)
{
  int width = pict.width();
  int height = pict.height();

  int linesize = getBufSize_(width * 3);

  std::unique_ptr<std::uint8_t[]> imgbuf;
  try {
    imgbuf = std::make_unique<std::uint8_t[]>(linesize * height);
  }
  catch(std::bad_alloc&) {
    return false;
  }

  is.read((char*)(imgbuf.get()), linesize * height);

  for (int y = height - 1, l = 0; y >= 0; --y, l += linesize) {
    auto lb = pict.lineBuffer(y);
    for (int x = 0, k = 0; x < width; ++x, k += 3) {
      lb[x].red = imgbuf[l + k + 2];
      lb[x].green = imgbuf[l + k + 1];
      lb[x].blue = imgbuf[l + k];
    }
  }

  return true;
}


/*
 * @brief 32bit/畫素のビットマップの讀み込み
 */
bool read32bit_(std::istream& is, eunomia::Picture& pict)
{
  int width = pict.width();
  int height = pict.height();

  int linesize = getBufSize_(width * 4);

  std::unique_ptr<std::uint8_t[]> imgbuf;
  try {
    imgbuf = std::make_unique<std::uint8_t[]>(linesize * height);
  }
  catch(std::bad_alloc&) {
    return false;
  }

  is.read((char*)(imgbuf.get()), linesize * height);

  for (int y = height - 1, l = 0; y >= 0; --y, l += linesize) {
    auto lb = pict.lineBuffer(y);
    for (int x = 0, k = 0; x < width; ++x, k += 4) {
      lb[x].red = imgbuf[l + k + 2];
      lb[x].green = imgbuf[l + k + 1];
      lb[x].blue = imgbuf[l + k];
    }
  }

  return true;
}


/*
 * @brief 1bit/畫素のビットマップの讀み込み
 */
bool read01bit_(std::istream& is, eunomia::PictureIndexed& pict)
{
  int width = pict.width();
  int bufsize = getBufSize_((width + 7) / 8);

  std::unique_ptr<std::uint8_t[]> linebuf;
  try {
    linebuf = std::make_unique<std::uint8_t[]>(bufsize);
  }
  catch(std::bad_alloc&) {
    return false;
  }

  for (int y = pict.height() - 1; y >= 0; --y) {
    if (!is.read((char*)(linebuf.get()), bufsize))
      return false;

    auto lb = pict.lineBuffer(y);
    read01_oneline_(linebuf.get(), width, lb);
  }

  return true;
}


/*
 * @brief 4bit/畫素のビットマップの讀み込み
 */
bool read04bit_(std::istream& is, eunomia::PictureIndexed& pict)
{
  int width = pict.width();
  int bufsize = getBufSize_((width + 1) / 2);
  std::unique_ptr<std::uint8_t[]> linebuf;
  try {
    linebuf = std::make_unique<std::uint8_t[]>(bufsize);
  }
  catch(std::bad_alloc&) {
    return false;
  }

  for (int y = pict.height() - 1; y >= 0; --y) {
    if (!is.read((char*)(linebuf.get()), bufsize))
      return false;
  
    auto lb = pict.lineBuffer(y);
    read04_oneline_(linebuf.get(), width, lb);
  }

  return true;
}


/*
 * @brief 8bit/畫素のビットマップの讀み込み
 */
bool read08bit_(std::istream& is, eunomia::PictureIndexed& pict)
{
  int width = pict.width();
  int bufsize = getBufSize_(width);
  std::unique_ptr<std::uint8_t[]> linebuf;
  try {
    linebuf = std::make_unique<std::uint8_t[]>(bufsize);
  }
  catch(std::bad_alloc&) {
    return false;
  }

  for (int y = pict.height() - 1; y >= 0; --y) {
    if (!is.read((char*)(linebuf.get()), bufsize))
      return false;
    std::copy_n(linebuf.get(), width, pict.lineBuffer(y));
  }

  return true;
}




//// PictureあるいはPictureIndexedを生成し、
//// 畫素あたりビット數に應じた讀み込み處理を呼び出す。

// 1bit, 4bit, 8bitのときはPictureIndexedを生成して讀み込み處理を行ふ。
bool
loadDibPictureIndexed_(
  std::istream& is,
  const Info_& info,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  upindx = eunomia::PictureIndexed::create(info.width, info.height);

  if (!upindx)
    return false;

  int np = (info.numpalette != 0) ? info.numpalette : (1 << info.bpp);
  if (!readPalette_(is, upindx->paletteBuffer(), np)) {
    upindx.reset();
    return false;
  }

  switch(info.bpp) {
  case 1:
    if (read01bit_(is, *upindx))
      return true;
    break;

  case 4:
    if (read04bit_(is, *upindx))
      return true;
    break;

  case 8:
    if (read08bit_(is, *upindx))
      return true;
    break;
  }

  // ここに到達するのは讀み込みに失敗したとき
  upindx.reset();
  return false;
}


// 24bit, 32bitのときはPictureを生成して讀み込み處理を行ふ。
bool
loadDibPicture_(
  std::istream& is,
  const Info_& info,
  std::unique_ptr<eunomia::Picture>& uppict)
{
  uppict = eunomia::Picture::create(info.width, info.height);

  if (uppict) {
    if (info.bpp == 24) {
      if (read24bit_(is, *uppict))
        return true;
    }
    else if (info.bpp == 32) {
      if (read32bit_(is, *uppict))
        return true;
    }

    // ここに到達するのは讀み込みに失敗したとき
    uppict.reset();
  }

  return false;
}


}//end of NONAME namespace




bool
eunomia::loadDib(
  const std::filesystem::path& path,
  std::unique_ptr<eunomia::Picture>& uppict,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  uppict.reset();
  upindx.reset();

  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  if (!ifs)
    return false;

  if (!readHeader_(ifs))
    return false;

  Info_ info;
  if (!readInfo_(ifs, info))
    return false;

  switch (info.bpp) {
  case 1:
  case 4:
  case 8:
    return loadDibPictureIndexed_(ifs, info, upindx);

  case 24:
  case 32:
    return loadDibPicture_(ifs, info, uppict);

  default:
    return false;
  }
}




//eof
