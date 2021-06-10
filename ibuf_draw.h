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
 * @file ibuf_draw.h
 * @author oZ/acy
 * @brief 畫像バッファクラステンプレートの描畫系のメンバ函數の實裝
 *
 * @date 2016.2.26 ellipse()内の不使用變數の宣言を削除
 * @date 2021.4.22 LIBPOLYMNIAからLIBEUNOMIAに移植
 * @date 2021.6.10 paintFill()内の不使用變數の宣言を削除
 *
 */
/* This file is included by "imagebuffer.h". */
#include <stack>
#include <cstdlib>
#include <algorithm>


// バッファ全體を塗り潰す
template<class C_>
inline void eunomia::ImageBuffer<C_>::clear(const C_& color)
{
  std::uint8_t lp = buffer();
  for (int j = 0; j < height(); ++j, lp += pitch())
    std::fill_n(reinterpret_cast<C_*>(lp), width(), color);
}


/*==========================================================
 *  直線を引く
 *  (Bresenham's Algorithm  參考: C MAGAZINE Dec. 2000)
 */
template<class C_>
inline
void
eunomia::ImageBuffer<C_>::line(int x1, int y1, int x2, int y2, const C_& color)
{
  if ((x1 < 0 && x2 < 0) || (x1 >= w_ && x2 >= w_)
      || (y1 < 0 && y2 < 0) || (y1 >= h_ && y2 >= h_))
    return;

  int dx, dy, sx, sy; // 差分と正負

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx < 0) {
    sx = -1;
    dx = -dx;
  }
  else if (dx == 0)
    sx = 0;
  else
    sx = 1;

  if (dy < 0) {
    sy = -1;
    dy = -dy;
  }
  else if (dy == 0)
    sy = 0;
  else
    sy = 1;

  // 描畫に伴ひ動かしてゆく變數
  int xx1 = x1;
  int xx2 = x2;
  int yy1 = y1;
  int yy2 = y2;

  if (dy == 0) {
    if (y1 >= 0 && y1 < h_) { // 要らない筈だがまあ……
      int left = std::max(0, std::min(x1, x2));
      int right = std::min(w_ - 1, std::max(x1, x2));
      std::fill_n(lineBuffer(y1) + left, right - left + 1, color);
    }
  }
  else if (dx == 0) {
    if (x1 >= 0 && x1 < w_) {  // 要らない筈だがまあ……
      int top = std::max(0, std::min(y1, y2));
      int bottom = std::min(h_ - 1, std::max(y1, y2));
      std::uint8_t* bp = buf_ + top * pitch_ + x1 * sizeof(C_);
      for (int j = top; j <= bottom; ++j, bp += pitch_)
        *reinterpret_cast<C_*>(bp) = color;
    }
  }
  else if (dx >= dy) {
    int e = -dx;
    int lx = (dx + 1) / 2;

    std::uint8_t* bp = buf_ + y1 * pitch_ + x1 * sizeof(C_);
    std::uint8_t* bq = buf_ + y2 * pitch_ + x2 * sizeof(C_);
    int spx = sx * sizeof(C_);

    for (int i = 0; i < lx; i++) {
      if (xx1 >= 0 && xx1 < w_ && yy1 >= 0 && yy1 < h_)
        *reinterpret_cast<C_*>(bp) = color;
      if (xx2 >= 0 && xx2 < w_ && yy2 >= 0 && yy2 < h_)
        *reinterpret_cast<C_*>(bq) = color;

      bp += spx;
      bq -= spx;

      xx1 += sx;
      xx2 -= sx;
      e += 2 * dy;
      if (e >= 0) {
        if (sy > 0) {
          bp += pitch_;
          bq -= pitch_;
          yy1++;
          yy2--;
        }
        else if (sy < 0) {
          bp -= pitch_;
          bq += pitch_;
          yy1--;
          yy2++;
        }

        e -= 2 * dx;
      }
    }

    if (!(dx % 2) && xx1 >= 0 && xx1 < w_ && yy1 >= 0 && yy1 < h_)
      *reinterpret_cast<C_*>(bp) = color;
  }
  else {
    int e = -dy;
    int ly = (dy + 1) / 2;
    std::uint8_t* bp = buf_ + y1 * pitch_ + x1 * sizeof(C_);
    std::uint8_t* bq = buf_ + y2 * pitch_ + x2 * sizeof(C_);
    int spx = sx * sizeof(C_);

    for (int i = 0; i < ly; i++) {
      if (xx1 >= 0 && xx1 < w_ && yy1 >= 0 && yy1 < h_)
        *reinterpret_cast<C_*>(bp) = color;
      if (xx2 >= 0 && xx2 < w_ && yy2 >= 0 && yy2 < h_)
        *reinterpret_cast<C_*>(bq) = color;

      if(sy > 0) {
        bp += pitch_;
        bq -= pitch_;
        yy1++;
        yy2--;
      }
      else if (sy < 0) {
        bp -= pitch_;
        bq += pitch_;
        yy1--;
        yy2++;
      }

      e += 2 * dx;
      if (e >= 0) {
        bp += spx;
        bq -= spx;
        xx1 += sx;
        xx2 -= sx;
        e -= 2 * dy;
      }
    }

    if (!(dy % 2) && xx1 >= 0 && xx1 < w_ && yy1 >= 0 && yy1 < h_)
      *reinterpret_cast<C_*>(bp) = color;
  }
}


/*================================================
 *  長方形を描く
 */
template<class C_>
inline
void
eunomia::ImageBuffer<C_>::box(
  int left, int top, int right, int bottom, const C_& color, bool fill)
{
  // 座標の簡單化
  if (left > right)
    std::swap(left, right);
  if (top > bottom)
    std::swap(top, bottom);

  // 範圍外なら何もしない
  if (left >= w_ || right < 0 || top >= h_ || bottom < 0)
    return;

  // 範圍内に切りつめた値
  int xx1 = std::max(left, 0);
  int xx2 = std::min(right, w_ - 1);
  int yy1 = std::max(top, 0);
  int yy2 = std::min(bottom, h_ - 1);

  if (fill) { // 塗り潰し
    std::uint8_t* lp = buf_ + pitch_ * yy1;
    for (int j = yy1; j <= yy2; ++j, lp += pitch_)
      std::fill_n(
        reinterpret_cast<C_*>(lp) + xx1, xx2 - xx1 + 1, color);
  }
  else {
    // 上邊
    if (top >= 0)
      std::fill_n(lineBuffer(top) + xx1, xx2 - xx1 + 1, color);
    // 下邊
    if (bottom < h_)
      std::fill_n(lineBuffer(bottom) + xx1, xx2 - xx1 + 1, color);

    std::uint8_t* lr = buf_ + yy1 * pitch_;
    for (int j = yy1; j <= yy2; ++j, lr += pitch_) {
      // 左邊
      if (left >= 0)
        reinterpret_cast<C_*>(lr)[left] = color;
      // 右邊
      if (right < w_)
        reinterpret_cast<C_*>(lr)[right] = color;
    }
  }
}


/*============================================
 *  楕円の描畫
 *  (Michener's Algorithm  參考: C MAGAZINE Jan. 2001)
 *
 *     1 ' 2
 *   3       4
 *   5       6       <-- コメント中で用いた番號
 *     7   8
 */
template<class C_>
inline
void
eunomia::ImageBuffer<C_>::ellipse(
  int x, int y, int a, int b, const C_& color, bool fill)
{
  if (a == 0 || b == 0)
    return;

  // 念の爲
  a = std::abs(a);
  b = std::abs(b);

  int p = 0;  // 半徑の短い方向の差分
  int q; // 半徑の長い方向の差分
  int e; // 判定用
  if (a < b) {
    e = 2 - 3 * b;
    q = b;
  }
  else {
    e = 2 - 3 * a;
    q = a;
  }

  while (p <= q) {
    // 實際に用ゐる中心座標との差分
    int dx1, dy1, dx2, dy2; 
    // 半徑の短い方向の差分を「潰」す
    if (a < b) {
      // 1, 2, 7, 8
      dx1 = p * a / b;
      dy1 = q;

      // 3, 4, 5, 6
      dx2 = q * a / b;
      dy2 = p;
    }
    else {
      dx1 = p;
      dy1 = q * b / a;
      
      dx2 = q;
      dy2 = p * b / a;
    }

    // 弧1, 2, 7, 8の處理
    if (x - dx1 < w_  &&  x + dx1 >= 0  &&  y - dy1 < h_  && y + dy1 >= 0) {
      // ↑まづは完全に畫像領域外でないことを確かめる

      if (fill) {
        int left = std::max(x - dx1, 0);
        int right = std::min(x + dx1, w_ - 1);

        if (y - dy1 >= 0) // 1<=>2
          std::fill_n(lineBuffer(y - dy1) + left, right - left + 1, color);
        if (y + dy1 < h_) // 7<=>8
          std::fill_n(lineBuffer(y + dy1) + left, right - left + 1, color);
      }
      else {
        if (y - dy1 >= 0) {
          if (x - dx1 >= 0)
            pixel(x - dx1, y - dy1) = color; // 1
          if (x + dx1 < w_)
            pixel(x + dx1, y - dy1) = color; // 2
        }
        if (y + dy1 < h_) {
          if (x - dx1 >= 0)
            pixel(x - dx1, y + dy1) = color; // 7
          if (x + dx1 < w_)
            pixel(x + dx1, y + dy1) = color; // 8
        }
      }
    }

    // 弧 3, 4, 5, 6の處理
    if (x - dx2 < w_  &&  x + dx2 >= 0  &&  y - dy2 < h_  &&  y + dy2 >= 0) {
      // ↑まづは完全に畫像領域外でないことを確かめる

      if (fill) {
        int left = std::max(x - dx2, 0);
        int right = std::min(x + dx2, w_ - 1);

        if (y - dy2 >= 0) // 3<=>4
          std::fill_n(lineBuffer(y - dy2) + left, right - left + 1, color);
        if (y + dy2 < h_) // 5<=>6
          std::fill_n(lineBuffer(y + dy2) + left, right - left + 1, color);
      }
      else {
        if (y - dy2 >= 0) {
          if (x - dx2 >= 0)
            pixel(x - dx2, y - dy2) = color; // 3
          if (x + dx2 < w_)
            pixel(x + dx2, y - dy2) = color; // 4
        }
        if (y + dy2 < h_) {
          if (x - dx2 >= 0)
            pixel(x - dx2, y + dy2) = color; // 5
          if (x + dx1 < w_)
            pixel(x + dx2, y + dy2) = color; // 6
        }
      }
    }

    // 次に進む
    if (e < 0)
      e += p * 4 + 6;
    else {
      e += (p - q) * 4 + 10;
      q--;
    }
    p++;
  }// while
}


/*================================================
 *  所謂塗り潰し
 */
template<class C_>
inline void eunomia::ImageBuffer<C_>::paintFill(int x, int y, const C_& color)
{
  if (x < 0 || y < 0 || x >= w_ || y >= h_)
    return;

  C_ fromcolor = pixel(x, y);
  if (color == fromcolor)
    return;

  std::stack<Point> pstack;
  pstack.push(Point(x, y));

  while (!pstack.empty()) {
    Point p = pstack.top();
    pstack.pop();

    int xx = p.x;
    int yy = p.y;

    // 上への進行を打診
    bool ful, fur; // 左上、右上方向の連續性の有無の確認用
    if (yy > 0 && pixel(xx, yy - 1) == fromcolor) {
      pstack.push(Point(xx, yy - 1));
      ful = true;
      fur = true;
    } else {
      ful = false;
      fur = false;
    }

    // 下への進行を打診
    bool fdl, fdr; // 左下、右下方向の連續性の有無の確認用
    if (yy < h_ - 1 && pixel(xx, yy + 1) == fromcolor) {
      pstack.push(Point(xx, yy + 1));
      fdl = true;
      fdr = true;
    }
    else {
      fdl = false;
      fdr = false;
    }

    // 現在の地點を塗る
    pixel(xx, yy) = color;

    C_* lb = lineBuffer(yy);
    C_* lbu = lineBuffer(yy - 1);
    C_* lbd = lineBuffer(yy + 1);

    // 左に進む
    for (int l = xx - 1; l >= 0 && lb[l] == fromcolor; --l) {
      // 左上側の處理
      if (yy > 0) {
        if (ful && lbu[l] != fromcolor)
          ful = false;
        else if (!ful && lbu[l] == fromcolor) {
          ful = true;
          pstack.push(Point(l, yy - 1));
        }
      }

      if (yy + 1 < h_) {
        if (fdl && lbd[l] != fromcolor)
          fdl = false;
        else if (!fdl && lbd[l] == fromcolor) {
          fdl = true;
          pstack.push(Point(l, yy + 1));
        }
      }

      lb[l] = color;
    }

    // 右に進む
    for (int r = xx + 1; r < w_ && lb[r] == fromcolor; ++r) {
      // 右上の處理
      if (yy > 0) {
        if (fur && lbu[r] != fromcolor)
          fur = false;
        else if (!fur && lbu[r] == fromcolor) {
          fur = true;
          pstack.push(Point(r, yy - 1));
        }
      }

      if (yy + 1 < h_) {
        if (fdr && lbd[r] != fromcolor)
          fdr = false;
        else if (!fdr && lbd[r] == fromcolor) {
          fdr = true;
          pstack.push(Point(r, yy + 1));
        }
      }

      lb[r] = color;
    }
  }
}




//eof
