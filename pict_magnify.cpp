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
 * @file pict_magnify.cpp
 * @author oZ/acy (名賀月晃嗣)
 * @brief Pictureの擴大處理 (biCubic法 參考: C MAGAZINE Oct. 1999)
 *
 * @date 27 Nov MMIV  擴大ルーチンのアルゴリズムをbiCubic法に變更
 * @date  8 Sep MMXI  NULL を nullptr に修正 (C++11)
 * @date  2 Mar MMXVI  throw()を削除
 * @date 29 Aug MMXIX  返却型を生ポインタからunique_ptrに變更
 *
 * @date 23 Apr MMXXI  LIBPOLYMNIAからLIBEUNOMIAへの移植とパラメタの追加
 *
 */
#include <algorithm>
#include "picture.h"

#include "pict_magnify_func.h"


std::unique_ptr<eunomia::Picture> 
eunomia::Picture::magnify(int w, int h, double a) const noexcept
{
  using eunomia::implement_::fCubic_;
  using eunomia::implement_::innerProduct_;
  using eunomia::implement_::productMat14_44_;

  auto pict = create(w, h);
  if (!pict)
    return nullptr;

  // X方向、Y方向の擴大率の逆數
  double nrx = (double)width() / (double)w;
  double nry = (double)height() / (double)h;

  for (int Y = 0; Y < h; Y++) { // Yは擴大畫像上のY座標
    double y0 = Y * nry;        // Yに對應する原畫像上のY座標
    double dy = y0 - (int)y0;   // その小數部分

    double fy[4];
    fy[0] = fCubic_(1.0 + dy, a);
    fy[1] = fCubic_(dy, a);
    fy[2] = fCubic_(1.0 - dy, a);
    fy[3] = fCubic_(2.0 - dy, a);

    int py[4]; // 原畫像上の近傍十六點(4*4)のY座標
    for (int i = 0; i < 4; i++) {
      py[i] = (int)y0 - 1 + i;

      // はみ出る分の處置
      if (py[i] < 0)
        py[i] = 0;
      else if (py[i] >= height())
        py[i] = height() - 1;
    }

    for (int X = 0; X < w; X++) { // Xは擴大畫像上のX座標
      double x0 = X * nrx;        // Xに對應する原畫像上のX座標
      double dx = x0 - (int)x0;   // その小數部分

      double fx[4];
      fx[0] = fCubic_(1.0 + dx, a);
      fx[1] = fCubic_(dx, a);
      fx[2] = fCubic_(1.0 - dx, a);
      fx[3] = fCubic_(2.0 - dx, a);

      int px[4]; // 原畫像上の近傍十六點(4*4)のX座標
      for (int i=0; i < 4; i++) {
        px[i] = (int)x0 - 1 + i;

        // はみ出る分の處置
        if (px[i] < 0)
          px[i] = 0;
        else if (px[i] >= width())
          px[i] = width() - 1;
      }

      double rbuf[16], gbuf[16], bbuf[16]; // 近傍十六點のRGB値を入れる4*4行列
      for (int i=0; i < 4; i++)
        for (int j=0; j < 4; j++)
        {
          rbuf[i + j * 4] = pixel(px[i], py[j]).red;
          gbuf[i + j * 4] = pixel(px[i], py[j]).green;
          bbuf[i + j * 4] = pixel(px[i], py[j]).blue;
        }

      double tmpR[4], tmpG[4], tmpB[4];
      productMat14_44_(tmpR, fy, rbuf);
      productMat14_44_(tmpG, fy, gbuf);
      productMat14_44_(tmpB, fy, bbuf);

      double R = innerProduct_(4, tmpR, fx);
      double G = innerProduct_(4, tmpG, fx);
      double B = innerProduct_(4, tmpB, fx);

      pict->pixel(X, Y).red = std::clamp(R, 0.0, 255.0);
      pict->pixel(X, Y).green = std::clamp(G, 0.0, 255.0);
      pict->pixel(X, Y).blue = std::clamp(B, 0.0, 255.0);

      /*
      std::uint8_t r, g, b;
      if (R < 0.0)
        r = 0;
      else if (R > 255.0)
        r = 255;
      else
        r = (std::uint8_t)R;

      if (G < 0.0)
        g = 0;
      else if (G > 255.0)
        g = 255;
      else
        g = (std::uint8_t)G;

      if (B < 0.0)
        b = 0;
      else if (B > 255.0)
        b = 255;
      else
        b = (std::uint8_t)B;

      pict->pixel(X, Y) = RgbColour(r, g, b);
      */
    }
  }

  return pict;
}



//eof
