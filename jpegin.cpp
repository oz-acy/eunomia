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
 * @file jpegin.cpp
 * @author oZ/acy (名賀月晃嗣)
 *
 */
#include <cstdio>
#include "jpegio.h"
#include "jpegio_implement.h"
#include "debuglogger.h"


/*
namespace
{

// オーバーロードによつて fopen、_wfopenを切り替へる
inline
std::FILE* openfile(const char* path)
{
  using namespace std;
  return fopen(path, "rb");
}
inline
std::FILE* openfile(const wchar_t* path)
{
  using namespace std;
  return _wfopen(path, L"rb");
}


}//end of namespace NONAME
*/



std::unique_ptr<eunomia::Picture>
eunomia::loadJpeg(const std::filesystem::path& path) noexcept
{
  //auto infile = openfile(path.c_str());
  auto infile = std::fopen(path.string().c_str(), "rb");
  if (!infile)
    return nullptr;

  // エラールーチンセットアップ
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  implement_::jpegErrorSetup_(jerr);

  try {
    jpeg_create_decompress(&cinfo); // cinfo情報のアロケートと初期化
    jpeg_stdio_src(&cinfo, infile); // データソースの指定
    jpeg_read_header(&cinfo, TRUE); // JPEGファイルのパラメータ情報の読み込み
    jpeg_start_decompress(&cinfo);  // 解凍開始

    if (cinfo.out_color_space != JCS_RGB) {
      DLOG << DLTSTMP << "色空間がRGBではなかつた。" << std::endl;
      throw implement_::JpegIOException_();
    }
    if (cinfo.output_components != 3) {
      DLOG << DLTSTMP << "チャネル數が3ではなかつた。" << std::endl;
      throw implement_::JpegIOException_();
    }

    auto q = Picture::create(cinfo.output_width, cinfo.output_height);
    if (!q) {
      DLOG << DLTSTMP << "Pictureの生成に失敗した。" << std::endl;
      throw implement_::JpegIOException_();
    }

    JSAMPROW buf[1];
    std::uint8_t* resbuf = q->buffer();
    int pitch = q->pitch();
    int j = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
      buf[0] = (JSAMPROW)(resbuf + j);
      jpeg_read_scanlines(&cinfo, buf, 1);
      j += pitch;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    std::fclose(infile);

    return q;
  }
  catch(implement_::JpegIOException_&)
  {
    jpeg_destroy_decompress(&cinfo);
    std::fclose(infile);
    return nullptr;
  }
}


//eof
