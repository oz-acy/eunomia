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
/**
 *  @file rect.h
 *  @author oZ/acy
 *  @brief 長方形表現型
 *
 *  @date 2021.6.9 作成開始
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_RECTANGLE_H
#define INCLUDE_GUARD_EUNOMIA_RECTANGLE_H

#include <algorithm>
#include <type_traits>

namespace eunomia
{

/**
 *  @brief 長方形
 */
struct Rect
{
  int left;   ///< 左端のX座標
  int top;    ///< 上端のY座標
  int right;  ///< 右端のX座標
  int bottom; ///< 下端のY座標

  Rect() = default;
  constexpr Rect(int l, int t, int r, int b) noexcept
    : left(l), top(t), right(r), bottom(b)
    {}

  void normalize()
    noexcept(
      std::is_nothrow_move_constructible<int>::value
      && std::is_nothrow_move_assignable<int>::value)
  {
    if (left > right)
      std::swap(left, right);
    if (top > bottom)
      std::swap(top, bottom);
  }

  constexpr bool isNormalized() const noexcept
  {
    return left <= right && top <= bottom;
  }

  constexpr int width() const noexcept { return right - left; }
  constexpr int height() const noexcept { return bottom - top; }
};


/**
 *  @brief 長方形の別表現
 */
struct AnotherRect
{
  int x;  ///< (左端の)X座標
  int y;  ///< (上端の)Y座標
  int width;   ///< 幅
  int height;  ///< 高さ

  AnotherRect() = default;
  constexpr AnotherRect(int xx, int yy, int w, int h) noexcept
    : x(xx), y(yy), width(w), height(h)
    {}
  constexpr explicit AnotherRect(const Rect& r) noexcept
    : x(std::min(r.left, r.right)), y(std::min(r.top, r.bottom)),
      width(std::max(r.left, r.right) - x), 
      height(std::max(r.top, r.bottom) - y)
    {}

  constexpr explicit operator bool() const noexcept
  {
    return width >= 0 && height >= 0;
  }

  constexpr operator Rect() const noexcept
  {
    return Rect(x, y, x + width, y + height);
  }
};


}//end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_RECTANGLE_H
