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
 * @file dibout.cpp
 * @author oZ/acy (名賀月晃嗣)
 * @brief DIB形式畫像ファイルの出力
 *
 * @date 2018.12.23 C++17對應
 * @date 2019.8.29 new[]をmake_uniqueに置換
 * @date 2021.4.29 LIBPOLYMNIAからLIBEUNOMIAに移植
 *
 */
#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include "dibio.h"


namespace
{

/*
 * @brief DIBヘッダの書き出し
 */
bool writeHeader_(std::ostream& os, int bitsize, int npal)
{
  std::uint32_t dword;
  std::uint16_t word;

  // マジックナンバー書き出し
  word = *(std::uint16_t*)("BM");
  os.write((char*)(&word), sizeof(std::uint16_t));

  // ファイルサイズ書き出し
  dword = 54 + bitsize + 4 * npal;
  os.write((char*)(&dword), sizeof(std::uint32_t));

  // 予約済ワード(2つ)を0で書き出し
  word = 0;
  os.write((char*)(&word), sizeof(std::uint16_t));
  os.write((char*)(&word), sizeof(std::uint16_t));

  // BitDataへのオフセット書き出し
  dword = 54 + 4 * npal;
  os.write((char*)(&dword), sizeof(std::uint32_t));

  return true;
}


/*
 * @brief BMP-Infoヘッダの書き出し
 */
bool writeInfo_(std::ostream& os, int w, int h, std::uint16_t bits, int npal)
{
  std::int32_t d;
  std::uint32_t u;
  std::uint16_t r;

  u
    =   5 * sizeof(std::uint32_t) 
      + 4 * sizeof(std::int32_t)
      + 2 * sizeof(std::uint16_t);
  os.write((char*)&u, sizeof(std::uint32_t));

  d = w;
  os.write((char*)&d, sizeof(std::int32_t));
  d = h;
  os.write((char*)&d, sizeof(std::int32_t));
  r = 1;
  os.write((char*)&r, sizeof(std::uint16_t));
  r = bits;
  os.write((char*)&r, sizeof(std::uint16_t));

  u = 0;
  os.write((char*)&u, sizeof(std::uint32_t));
  os.write((char*)&u, sizeof(std::uint32_t));
  d = 0;
  os.write((char*)&d, sizeof(std::int32_t));
  os.write((char*)&d, sizeof(std::int32_t));

  u = npal;
  os.write((char*)&u, sizeof(std::uint32_t));
  u = 0;
  os.write((char*)&u, sizeof(std::uint32_t));

  return true;
}


/*
 * @brief パレットの書き出し
 */
bool writePalette_(std::ostream& os, const eunomia::RgbColour pal[], int np)
{
  struct
  {
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t v;
  } quad[256];

  for (int i = 0; i < np; i++) {
    quad[i].r = pal[i].red;
    quad[i].g = pal[i].green;
    quad[i].b = pal[i].blue;
    quad[i].v = 0;
  }

  os.write((char*)quad, sizeof(std::uint8_t) * 4 * np);

  return true;
}


/*
 * @brief PictureIndexedの各畫素の書き出し
 */
bool writeBits_(
  std::ostream& os, const eunomia::PictureIndexed& pict, int bufsize)
{
  auto linebuf = std::make_unique<std::uint8_t[]>(bufsize);
  std::fill_n(linebuf.get(), bufsize, 0);

  int w = pict.width();
  for (int j = pict.height() - 1; j >= 0; --j) {
    std::copy_n(pict.lineBuffer(j), w, linebuf.get());
    os.write((char*)(linebuf.get()), bufsize);
  }

  return true;
}


/* 24bit Bitmapデータ書き出し */
bool
writeBits_(
  std::ostream& os, const eunomia::Picture& pict, int bufsize)
{
  auto linebuf = std::make_unique<std::uint8_t[]>(bufsize);
  std::fill_n(linebuf.get(), bufsize, 0);

  int w = pict.width();
  for (int j = pict.height() - 1; j >= 0; --j) {
    const auto* lb = pict.lineBuffer(j);
    for (int i = 0; i < w; ++i) {
      const auto& c = lb[i];
      linebuf[3 * i] = c.blue;
      linebuf[3 * i + 1] = c.green;
      linebuf[3 * i + 2] = c.red;
    }
    os.write((char*)(linebuf.get()), bufsize);
  }

  return true;
}


}// end of NONAME namespace


bool
eunomia::saveDib(
  const eunomia::Picture& pict, const std::filesystem::path& path)
{
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  if (!ofs)
    return false;

  int bufsize = pict.width() * 3;
  bufsize = ((bufsize + 3) / 4) * 4;

  int mapsize = bufsize * pict.height();

  writeHeader_(ofs, mapsize, 0);
  writeInfo_(ofs, pict.width(), pict.height(), 24, 0);
  writeBits_(ofs, pict, bufsize);

  return true;
}


bool
eunomia::saveDib(
  const eunomia::PictureIndexed& pict, const std::filesystem::path& path)
{
  using namespace std;

  std::ofstream ofs(path, ios::out|ios::binary);
  if (!ofs)
    return false;

  int bufsize = pict.width();
  bufsize = ((bufsize + 3) / 4) * 4;

  int mapsize = bufsize * pict.height();

  writeHeader_(ofs, mapsize, 256);
  writeInfo_(ofs, pict.width(), pict.height(), 8, 0);
  writePalette_(ofs, pict.paletteBuffer(), 256);
  writeBits_(ofs, pict, bufsize);

  return true;
}




//eof
