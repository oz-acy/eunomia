/*
 * Copyright 2001-2021 oZ/acy (名賀月晃嗣)
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
 * @file picture_indexed.cpp
 * @author oZ/acy
 * 
 * @date 2016.3.2 C++11對應(假)
 * @date 2018.12.27 catch(bad_alloc)をcatch(bad_alloc&)に修正
 * @date 2019.8.29 create系統の返却型をunique_ptrに變更
 *
 * @date 2021.4.24 LIBPOLYMNIAからLIBEUNOMIAに移植
 *
 */

#include <algorithm>
#include <new>
#include "picture_indexed.h"


eunomia::PictureIndexed::PictureIndexed(unsigned w, unsigned h)
  :
  ImageBuffer<std::uint8_t>(w, h, sizeof(std::uint8_t) * w),
  upbuf_(std::make_unique<std::uint8_t[]>(w * h))
{
  buf_ = upbuf_.get();
}


std::unique_ptr<eunomia::PictureIndexed>
eunomia::PictureIndexed::create(unsigned w, unsigned h) noexcept
{
  try {
    return std::unique_ptr<PictureIndexed>(new PictureIndexed(w, h));
  }
  catch(std::bad_alloc&) {
    return nullptr;
  }
}


std::unique_ptr<eunomia::PictureIndexed>
eunomia::PictureIndexed::clone() const noexcept
{
  auto res = create(w_, h_);
  if (res) {
    std::copy_n(buf_, h_ * pitch_, res->buf_);
    std::copy_n(pal_, 256, res->pal_);
  }
  return res;
}




//eof
