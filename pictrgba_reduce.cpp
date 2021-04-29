/*
 * Copyright 2003-2021 oZ/acy (名賀月晃嗣)
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
 * @file pictrgba_reduce.cpp
 * @author oZ/acy (名賀月晃嗣)
 * @brief PictureRgbaの縮小處理 (參考: C MAGAZINE Oct. 1999)
 *
 * @date 24 Apr MMXXI  Picture::reduceを改作
 *
 */
#include <cmath>
#include "picture_rgba.h"


namespace {

// 元畫像の(x1, y1)-(x2, y2)を一點に「凝縮」する
eunomia::RgbaColour
condense_(
  const eunomia::PictureRgba& src, double x1, double y1, double x2, double y2)
{
  // 領域のbeginとendになる座標
  int bx = (int)x1; // or std::floor(x1);
  int by = (int)y1; // or std::floor(y1);
  int ex = std::ceil(x2);
  int ey = std::ceil(y2);

  double S = 0.0;  // 總面積
  double R = 0.0;
  double G = 0.0;
  double B = 0.0;
  double A = 0.0;

  for (int y = by; y < ey; ++y) {

    // y1〜y2の範圍を、整數目盛で區切つて調べる

    // 今回調べる範圍の上端と下端
    double top = y;
    double bottom = y + 1;
    if (top < y1)
      top = y1;
    if (bottom > y2)
      bottom = y2;

    double ph = bottom - top;  // 高さ

    for (int x = bx; x < ex; ++x) {

      // x1〜x2の範圍を、整數目盛で區切つて調べる

      // 今回調べる範圍の左端と右端
      double left = x;
      double right = x + 1;
      if (left < x1)
        left = x1;
      if (right > x2)
        right = x2;

      double pw = right - left;  // 幅

      double ss = ph * pw;  // 今回調べてゐる部分の面積
      S += ss;

      R += ss * src.pixel(x, y).red;
      G += ss * src.pixel(x, y).green;
      B += ss * src.pixel(x, y).blue;
      A += ss * src.pixel(x, y).alpha;
    }
  }

  if (S == 0.0)
    return eunomia::RgbaColour(0, 0, 0, 0);
  else
    return
      eunomia::RgbaColour(
        (std::uint8_t)(R / S + 0.5),
        (std::uint8_t)(G / S + 0.5),
        (std::uint8_t)(B / S + 0.5),
        (std::uint8_t)(A / S + 0.5));
}


}//end of namespace


std::unique_ptr<eunomia::PictureRgba>
eunomia::PictureRgba::reduce(int w, int h) const noexcept
{
  auto pict = create(w, h);
  if (!pict)
    return nullptr;

  // 元サイズ/縮小サイズ
  double dw = (double)width() / (double)w;
  double dh = (double)height() / (double)h;

  for (int Y = 0; Y < h; Y++) { // Yは縮小畫像上の座標
    // Yに對應する原畫像上の座標
    double y1 = Y * dh;
    double y2 = (Y + 1) * dh;
    
    if (y2 > height())
      break;

    for (int X = 0; X < w; X++) { // Xは縮小畫像上の座標
      // Xに對應する原畫像上の座標
      double x1 = (double)X * dw;
      double x2 = (double)(X + 1) * dw;

      if (x2 > width())
        break;

      pict->pixel(X, Y) = condense_(*this, x1, y1, x2, y2);
    }
  }

  return pict;
}




//eof
