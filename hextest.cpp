/*
 * Copyright 2016-2021 oZ/acy (名賀月晃嗣)
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
/**
 * @file hextest.cpp
 * @author oZ/acy
 * @brief HexPainterのテスト用のプログラム
 *
 * @date 2021.4.28 LIBEUNOMIAに追加
 *
 */

#include <iostream>
#include <memory>
#include <cmath>
#include <cstdlib>
#include "pngio.h"
#include "hexpainter.h"


namespace {
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

constexpr int R = 32;
constexpr int P0 = 50;
constexpr int Q0 = 50;
constexpr int MAX_XY = 16;
constexpr int STEP = 10;
}


int main(int argc, char** argv)
{
  // 出力對象
  auto pict = eunomia::Picture::create(WIDTH, HEIGHT);
  eunomia::HexPainter<eunomia::RgbColour> hp(R, P0, Q0);

  for (int q = 0; q < HEIGHT; q++) {
    for (int p = 0; p < WIDTH; p++) {
      int x, y;
      hp.getHexPosition(p, q, x, y);

      if ((x < 0) || (y < 0) || (x >= MAX_XY) || (y >= MAX_XY)) {
        pict->pixel(p, q) = eunomia::RgbColour(255, 255, 255);
      }
      else {

        // a, b, c を求める
        int a = x - (y / 2);
        int b = y;
        int c = a + b;

        int red = ((a * STEP) + 128) & 0x00FF;
        int blue = (b * STEP) & 0x00FF;
        int green = (c * STEP) & 0x00FF;

        pict->pixel(p, q) = eunomia::RgbColour(red, green, blue);
      }
    }
  }

  pict->pixel(P0, Q0) = eunomia::RgbColour(0, 0, 0);
  
  hp.draw(*pict, 1, 1, eunomia::RgbColour(128, 128, 128));
  hp.draw(*pict, 1, 2, eunomia::RgbColour(0, 0, 0));
  hp.draw(*pict, 2, 1, eunomia::RgbColour(0, 0, 255));
  hp.draw(*pict, 2, 2, eunomia::RgbColour(0, 255, 0));

  hp.fill(*pict, 4, 4, eunomia::RgbColour(128, 128, 128));
  hp.draw(*pict, 4, 4, eunomia::RgbColour(255, 0, 0));

  hp.fill(*pict, 4, 6, 4, 4, eunomia::RgbColour(0, 0, 128));
  hp.draw(*pict, 4, 6, 4, 4, eunomia::RgbColour(255, 0, 128));
  // pictをファイルに出力して終はり
  eunomia::savePng(*pict, "hextest.png");

  return 0;
}


//eof
