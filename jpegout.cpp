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
 * @file jpegout.cpp
 * @author oZ/acy
 *
 * @date 2021.4.29 LIBPOLYMNIAから改作
 *
 */

#include <cstdio>
#include "jpegio.h"
#include "jpegio_implement.h"


namespace {

// 多重定義(オーバーロード)によつてfopen、_wfopenを切り替へる
inline
std::FILE* openfile(const char* path)
{
  using namespace std;
  return fopen(path, "wb");
}

inline
std::FILE* openfile(const wchar_t* path)
{
  using namespace std;
  return _wfopen(path, L"wb");
}

}// end of NONAME namespace 




bool
eunomia::saveJpeg(
  const eunomia::Picture& pict, const std::filesystem::path& path,
  bool prog, int quality) noexcept
{
  auto outfile = openfile(path.c_str());
  if (!outfile)
    return false;

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  implement_::jpegErrorSetup_(jerr);

  try {
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = pict.width();
    cinfo.image_height = pict.height();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    if (prog)
      jpeg_simple_progression(&cinfo);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW buf[1];
    const std::uint8_t* srcbuf = pict.buffer();
    int pitch = pict.pitch();
    int j = 0;
    while (cinfo.next_scanline < cinfo.image_height) {
      buf[0] = (JSAMPROW)(srcbuf + j);
      jpeg_write_scanlines(&cinfo, buf, 1);
      j += pitch;
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    std::fclose(outfile);

    return true;
  }
  catch(implement_::JpegIOException_&) {
    jpeg_destroy_compress(&cinfo);
    std::fclose(outfile);
    return false;
  }
}




//eof
