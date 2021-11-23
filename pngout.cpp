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
 * @file pngout.cpp
 * @author oZ/acy
 * @brief PNG形式畫像ファイルの書き込み
 *
 * @date 2021.4.26 LIBPOLYMNIAのPNG書き込み處理から改作
 * @date 2021.11.23 PictureIndexed向けのsavePng()の仕樣を變更
 *
 */
#include <iostream>
#include <fstream>

extern "C" {
#include <png.h>
}

#include "pngio.h"


namespace
{

/**
 * @brief std::ostreamへの書き込み
 *
 * @date H23.4.24 update for libpng-1.5.x
 */
void pngWrite_(png_structp ppng, png_bytep data, png_size_t length)
{
  std::ostream* pos = reinterpret_cast<std::ostream*>(png_get_io_ptr(ppng));
  pos->write(reinterpret_cast<char*>(data), length);
}


/**
 * @brief std::ostreamのflush
 */
void pngFlush_(png_structp ppng)
{
  std::ostream* pos = reinterpret_cast<std::ostream*>(png_get_io_ptr(ppng));
  pos->flush();
}


/**
 * @brief 書き込みの初期處理
 */
bool pngWriteInit_(std::ostream& os, png_structp& ppng, png_infop& ppnginfo)
{
  ppng
    = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!ppng)
    return false;

  ppnginfo = png_create_info_struct(ppng);
  if (!ppnginfo) {
    png_destroy_write_struct(&ppng, nullptr);
    return false;
  }

  // 書き込み用の函數と引き渡すパラメタ(出力ストリーム)の設定
  png_set_write_fn(ppng, (void *)&os, pngWrite_, pngFlush_);

  return true;
}


}//end of NONAME namespace


bool
eunomia::savePng(
  const eunomia::Picture& pict, const std::filesystem::path& path)
{
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  if (!ofs)
    return false;

  png_structp ppng;
  png_infop ppnginfo;
  if (!pngWriteInit_(ofs, ppng, ppnginfo))
    return false;

  constexpr int bitdepth = 8;
  constexpr int colortype = PNG_COLOR_TYPE_RGB;
  constexpr int itype = PNG_INTERLACE_NONE;
  //int itype = interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(
    ppng, ppnginfo,
    pict.width(), pict.height(), bitdepth, colortype, itype,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // 各種情報の書き込み
  png_write_info(ppng, ppnginfo);

  // インターレース化する場合のパス數を取得 (しない場合は1固定)
  //int n_pass;
  //if (interlace)
  //  n_pass = png_set_interlace_handling(png_ptr);
  //else
  //  n_pass = 1;

  //for (int pass=0; pass < n_pass; pass++) // インターレースを可とするなら...
    for (int y = 0; y < pict.height(); ++y)
      png_write_row(ppng, (png_bytep)pict.lineBuffer(y));

  png_write_end(ppng, ppnginfo);
  png_destroy_write_struct(&ppng, &ppnginfo);

  return true;
}


bool
eunomia::savePng(
  const eunomia::PictureRgba& pict, const std::filesystem::path& path)
{
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  if (!ofs)
    return false;

  png_structp ppng;
  png_infop ppnginfo;
  if (!pngWriteInit_(ofs, ppng, ppnginfo))
    return false;

  constexpr int bitdepth = 8;
  constexpr int colortype = PNG_COLOR_TYPE_RGB_ALPHA;
  constexpr int itype = PNG_INTERLACE_NONE;
  //int itype = interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(
    ppng, ppnginfo,
    pict.width(), pict.height(), bitdepth, colortype, itype,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // 各種情報の書き込み
  png_write_info(ppng, ppnginfo);

  // インターレース化する場合のパス數を取得 (しない場合は1固定)
  //int n_pass;
  //if (interlace)
  //  n_pass = png_set_interlace_handling(png_ptr);
  //else
  //  n_pass = 1;

  //for (int pass=0; pass < n_pass; pass++) // インターレースを可とするなら...
    for (int y = 0; y < pict.height(); ++y)
      png_write_row(ppng, (png_bytep)pict.lineBuffer(y));

  png_write_end(ppng, ppnginfo);
  png_destroy_write_struct(&ppng, &ppnginfo);

  return true;
}


bool
eunomia::savePng(
  const eunomia::PictureIndexed& pict, const std::filesystem::path& path,
  bool trns, std::uint8_t tpal)
{
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  if (!ofs)
    return false;

  png_structp ppng;
  png_infop ppnginfo;
  if (!pngWriteInit_(ofs, ppng, ppnginfo))
    return false;

  constexpr int bitdepth = 8;
  constexpr int colortype = PNG_COLOR_TYPE_PALETTE;
  constexpr int itype = PNG_INTERLACE_NONE;
  //int itype = interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(
    ppng, ppnginfo,
    pict.width(), pict.height(), bitdepth, colortype, itype,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  // パレットの設定
  auto palette = std::make_unique<png_color[]>(256);
  for (int i = 0; i < 256; i++) {
    palette[i].red = pict.palette(i).red;
    palette[i].green = pict.palette(i).green;
    palette[i].blue = pict.palette(i).blue;
  }
  png_set_PLTE(ppng, ppnginfo, palette.get(), 256);

  //透過処理
  auto transbf = std::make_unique<png_byte[]>(256);
  if (trns) {
    for (int i = 0; i < 256; i++)
      if (i == tpal)
        transbf[i] = 0;
      else
        transbf[i] = 0xff;
    png_set_tRNS(ppng, ppnginfo, transbf.get(), 256, nullptr);
  }

  // 各種情報の書き込み
  png_write_info(ppng, ppnginfo);

  // インターレス化する場合の繰り返し囘數を取得 (しない場合は1固定)
  //int n_pass;
  //if(interlace)
  //  n_pass = png_set_interlace_handling(png_ptr);
  //else
  //  n_pass = 1;

  //for (int pass = 0; pass < n_pass; pass++)
    for (int y = 0; y < pict.height(); ++y)
      png_write_row(ppng, (png_bytep)pict.lineBuffer(y));

  png_write_end(ppng, ppnginfo);
  png_destroy_write_struct(&ppng, &ppnginfo);

  return true;
}


//eof
