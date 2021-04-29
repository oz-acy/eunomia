/*
 * Copyright 2008-2021 oZ/acy (名賀月晃嗣)
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
 * @file pictrgba_grayscaled.cpp
 * @author oZ/acy
 *
 * @date 2021.4.24 Picture::createGrayscaledPictureIndexedを改作
 *
 */
#include "picture_rgba.h"
#include "picture_indexed.h"


std::unique_ptr<eunomia::PictureIndexed>
eunomia::PictureRgba::createGrayscaledPictureIndexed() const noexcept
{
  auto pi = PictureIndexed::create(w_, h_);
  if (pi) {
    // パレットの設定
    for (int i = 0; i < 256; i++)
      pi->palette(i) = RgbColour(i, i, i);

    // グレイスケール化
    for (int y = 0; y < h_; ++y) {
      for (int x = 0; x < w_; ++x) {
        const auto& p = pixel(x, y);
        std::uint8_t l = p.red * 0.2990 + p.green * 0.5870 + p.blue * 0.1140;
        pi->pixel(x, y) = l;
      }
    }
  }
  return pi;
}




//eof
