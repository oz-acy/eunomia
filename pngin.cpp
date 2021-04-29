/*
 * Copyright 2021 oZ/acy (名賀月晃嗣)
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
 * @file pngin.cpp
 * @author oZ/acy (名賀月晃嗣)
 * @brief PNG形式畫像ファイルの讀み込み
 *
 * @date 2021.4.26 LIBPOLYMNIAのPNG讀み込み處理から改作
 */
#include <iostream>
#include <fstream>

extern "C" {
#include <png.h>
}

#include "debuglogger.h"
#include "exception.h"
#include "pngio.h"


namespace
{
/**
 * @brief 讀み込み時の例外
 */
class PngReadException_ : public eunomia::Exception
{
public:
  PngReadException_() : Exception("PngReadException_") {}
};


// ファイル先頭のシグネチャ(チェックバイト)の大きさ
constexpr int PNG_CHECK_BYTES_ = 8; 

/**
 * @brief PNGのシグネチャの確認
 *
 * ファイル先頭のシグネチャを調べてPNGファイルか否か確認する。
 *
 * 確認の後、入力ストリームの入力位置を元に戾す。
 */
bool isPng_(std::istream& is)
{
  // ファイルの先頭8バイトを調べる
  unsigned char buf[PNG_CHECK_BYTES_];

  if (is.read((char*)buf, sizeof(buf)))
    return !png_sig_cmp(buf, 0, PNG_CHECK_BYTES_);
  else
    return false;

  //auto pos = is.tellg();
  //if (is.read((char*)buf, sizeof(buf))) {
  //  is.seekg(pos);
  //  return !png_sig_cmp(buf, 0, PNG_CHECK_BYTES_);
  //}
  //else {
  //  is.seekg(pos);
  //  return false;
  //}
}


/**
 * @brief std::istreamからの讀み込み
 *
 * @date H23.4.24 update for libpng-1.5.x
 */
void pngRead_(png_structp ppng, png_bytep data, png_size_t length)
{
  std::istream* pis = reinterpret_cast<std::istream*>(png_get_io_ptr(ppng));
  pis->read(reinterpret_cast<char*>(data), length);
}


/**
 * @brief エラー處理用函數
 *
 * libpngがエラーを起こしたときに呼び出す
 */
void pngError_(png_structp png_ptr, png_const_charp str)
{
  DLOG << DLTSTMP << str << std::endl;
  throw PngReadException_();
}


/**
 * @brief 讀み込み處理の初期化
 */
bool
pngReadInit_(
  std::istream& is, png_structp& ppng, png_infop& ppnginfo)
{
  using namespace std;

  // シグネチャのチェック
  if (!isPng_(is))
    return false;

  // PNG入力用構造体の生成
  ppng
    = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, pngError_, pngError_);
  if (!ppng)
    return false;

  ppnginfo = png_create_info_struct(ppng);
  if (!ppnginfo)
  {
    png_destroy_read_struct(&ppng, nullptr, nullptr);
    return false;
  }

  // 入力函數と引き渡すパラメタ(入力ストリーム)の設定
  png_set_read_fn(ppng, &is, pngRead_);

  // シグネチャを讀み込んだ分の通知
  png_set_sig_bytes(ppng, PNG_CHECK_BYTES_);

  return true;
}




/**
 * @brief PictureIndexedの生成と讀み込み
 */
void
loadPngPictureIndexed_(
  png_structp ppng, png_infop ppnginfo,
  unsigned width, unsigned height, bool isgray,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  // インターレース畫像の處置
  int npass = png_set_interlace_handling(ppng); 

  // ここまで設定した入力変換を反映させる
  png_read_update_info(ppng, ppnginfo);

  upindx = eunomia::PictureIndexed::create(width, height);
  if (!upindx)
    throw PngReadException_();

  // パレットの設定
  if (isgray) { //グレイスケールなら自分で用意する
    for (int i = 0; i < 256; ++i)
      upindx->palette(i) = eunomia::RgbColour(i, i, i);
  }
  else { // パレットを讀み込む
    int npal;
    png_colorp ppal;
    png_get_PLTE(ppng, ppnginfo, &ppal, &npal);

    for (int i = 0; i < npal; ++i)
      upindx->palette(i)
        = eunomia::RgbColour(ppal[i].red, ppal[i].green, ppal[i].blue);
  }

  // インデックスあるいはグレイ値の讀み込み
  for (int pass = 0; pass < npass; ++pass)
    for (unsigned y = 0; y < height; ++y) {
      png_read_row(ppng, nullptr, upindx->lineBuffer(y));
    }
}


/**
 * @brief Pictureの生成と讀み込み
 */
void
loadPngPicture_(
  png_structp ppng, png_infop ppnginfo,
  unsigned width, unsigned height, std::unique_ptr<eunomia::Picture>& uppict)
{
  // インターレース畫像の處置
  int npass = png_set_interlace_handling(ppng); 

  // ここまで設定した入力変換を反映させる
  png_read_update_info(ppng, ppnginfo);

  uppict = eunomia::Picture::create(width, height);
  if (!uppict)
    throw PngReadException_();

  for (int pass = 0; pass < npass; ++pass)
    for (unsigned y = 0; y < height; ++y)
      png_read_row(ppng, (png_bytep)(uppict->lineBuffer(y)), nullptr);
}


/**
 * @brief PictureRgbaの生成と讀み込み
 */
void
loadPngPictureRgba_(
  png_structp ppng, png_infop ppnginfo,
  unsigned width, unsigned height,
  std::unique_ptr<eunomia::PictureRgba>& uprgba)
{
  // インターレース畫像の處置
  int npass = png_set_interlace_handling(ppng); 

  // ここまで設定した入力変換を反映させる
  png_read_update_info(ppng, ppnginfo);

  uprgba = eunomia::PictureRgba::create(width, height);
  if (!uprgba)
    throw PngReadException_();

  for (int pass = 0; pass < npass; ++pass)
    for (unsigned y = 0; y < height; ++y)
      png_read_row(ppng, (png_bytep)(uprgba->lineBuffer(y)), nullptr);
}




/**
 * @brief パレットカラー畫像の讀み込み
 */
void
loadPngPalette_(
  png_structp ppng, png_infop ppnginfo,
  unsigned width, unsigned height, int bitdepth,
  std::unique_ptr<eunomia::Picture>& uppict,
  std::unique_ptr<eunomia::PictureRgba>& uprgba,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  if (png_get_valid(ppng, ppnginfo, PNG_INFO_tRNS)) {
    // 將來的には透過色情報をPictureIndexedに持たせることも考慮したいが
    // 取り敢へずは32bitRGBAに直す。
    png_set_palette_to_rgb(ppng);
    png_set_tRNS_to_alpha(ppng);
    
    // PictureRgbaの讀み込み處理を呼ぶ
    loadPngPictureRgba_(ppng, ppnginfo, width, height, uprgba);
  }
  else {
    if (bitdepth < 8)
      png_set_packing(ppng); // PAL8bitに統一

    loadPngPictureIndexed_(ppng, ppnginfo, width, height, false, upindx);
  }
}


/**
 * @brief グレイスケール畫像の讀み込み
 */
void
loadPngGray_(
  png_structp ppng, png_infop ppnginfo, 
  unsigned width, unsigned height, int bitdepth, bool alpha,
  std::unique_ptr<eunomia::Picture>& uppict,
  std::unique_ptr<eunomia::PictureRgba>& uprgba,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  // 畫素値を8bitに揃へる
  if (bitdepth == 16)
    png_set_strip_16(ppng);
  else if (bitdepth < 8)
    png_set_expand_gray_1_2_4_to_8(ppng);

  if (!alpha && png_get_valid(ppng, ppnginfo, PNG_INFO_tRNS)) {
    // 將來的には透過色情報をPictureIndexedに持たせることも考慮したいが
    // 取り敢へずはαチャネルを加へる方針でゆく。
    png_set_tRNS_to_alpha(ppng);
    alpha = true;
  }

  if (alpha) {
    png_set_gray_to_rgb(ppng);

    // PictureRgba用の處理
    loadPngPictureRgba_(ppng, ppnginfo, width, height, uprgba);
  }
  else {
    // PictureIndexedを生成する
    loadPngPictureIndexed_(ppng, ppnginfo, width, height, true, upindx);
  }
}


/**
 * @brief RGB(A)畫像の讀み込み
 */
void
loadPngRgb_(
  png_structp ppng, png_infop ppnginfo,
  unsigned width, unsigned height, int bitdepth, bool alpha,
  std::unique_ptr<eunomia::Picture>& uppict,
  std::unique_ptr<eunomia::PictureRgba>& uprgba,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  // 色要素深さが16bitのものは8bitに切り詰める。
  if (bitdepth > 8) // or bitdepth == 16
    png_set_strip_16(ppng); 

  if (!alpha && png_get_valid(ppng, ppnginfo, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(ppng);
    alpha = true;
  }

  if (alpha) {
    // PictureRgba用の處理
    loadPngPictureRgba_(ppng, ppnginfo, width, height, uprgba);
  }
  else {
    // Picture用の處理
    loadPngPicture_(ppng, ppnginfo, width, height, uppict);
  }
}


}// end of namespace NONAME


bool eunomia::loadPng(
  const std::filesystem::path& path,
  std::unique_ptr<Picture>& uppict,
  std::unique_ptr<PictureRgba>& uprgba,
  std::unique_ptr<PictureIndexed>& upindx)
{
  uppict.release();
  uprgba.release();
  upindx.release();

  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  if (!ifs)
    return false;

  png_structp ppng;
  png_infop ppnginfo;

  if (!pngReadInit_(ifs, ppng, ppnginfo))
    return false;

  try {
    // 情報の讀み込み
    png_read_info(ppng, ppnginfo);

    // IHDRチャンクの讀み込み
    png_uint_32 width, height;
    int bitdepth, colortype;

    png_get_IHDR(
      ppng, ppnginfo, &width, &height, 
      &bitdepth, &colortype, nullptr, nullptr, nullptr);


    // colortypeに應じた處理を行ふ
    switch (colortype) {
    case PNG_COLOR_TYPE_PALETTE:
      loadPngPalette_(
        ppng, ppnginfo, width, height, bitdepth, uppict, uprgba, upindx);
      break;

    case PNG_COLOR_TYPE_GRAY:
      loadPngGray_(
        ppng, ppnginfo, width, height, bitdepth, false, uppict, uprgba, upindx);
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      loadPngGray_(
        ppng, ppnginfo, width, height, bitdepth, true, uppict, uprgba, upindx);
      break;

    case PNG_COLOR_TYPE_RGB:
      loadPngRgb_(
        ppng, ppnginfo, width, height, bitdepth, false,
        uppict, uprgba, upindx);
      break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
      loadPngRgb_(
        ppng, ppnginfo, width, height, bitdepth, true, uppict, uprgba, upindx);
      break;

    default:
      throw PngReadException_();
      break;
    }

    png_read_end(ppng, ppnginfo);
    png_destroy_read_struct(&ppng, &ppnginfo, nullptr);

    return true;
  }
  catch (PngReadException_&) {
    png_destroy_read_struct(&ppng, &ppnginfo, nullptr);
    uppict.release();
    uprgba.release();
    upindx.release();
    return false;
  }
}




//eof
