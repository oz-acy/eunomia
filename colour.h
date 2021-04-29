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
/**
 *  @file colour.h
 *  @author oZ/acy
 *  @brief RGB24bit色及びRGBA32bit色
 *
 *  @date 2016.3.2  C++11/14對應(假)
 *  @date 2021.3.25 <cstdint>に定義された整数型を直接參照するやうに修正
 *
 *  @date 2021.4.29 v0.1
 *    - LIBPOLYMNIAのRGB24bit色情報クラスを移植(RgbColour)
 *    - RGB32bit色情報クラスを追加(RgbaColour)
 *    - RgbColour と RgbaColour との等價比較演算、非等價比較演算の追加
 *    - ImageBuffer<> で用ゐるための函數オブジェクトクラスの追加
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_COLOUR_H
#define INCLUDE_GUARD_EUNOMIA_COLOUR_H

#include <algorithm>
#include <cstdint>

namespace eunomia
{
/**
 * @brief RGB24bit色情報クラス
 */
class RgbColour
{
public:
  std::uint8_t red;   ///< 赤要素
  std::uint8_t green; ///< 緑要素
  std::uint8_t blue;  ///< 青要素

public:
  /// @brief 構築子
  ///
  /// RGB(0, 0, 0)で初期化する。
  constexpr RgbColour() noexcept : red(0), green(0), blue(0) {}

  /// @brief 構築子
  ///
  /// RGB値を指定して初期化する。
  constexpr 
  RgbColour(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
    : red(r), green(g), blue(b)
    {}

  /// @brief 等價比較演算子
  constexpr
  bool operator==(const RgbColour& other) const noexcept
    { return red == other.red && green == other.green && blue == other.blue; }

  /// @brief 非等價比較演算子
  constexpr
  bool operator!=(const RgbColour& other) const noexcept
    { return !(*this == other); }
};


/**
 * @brief RGBA32bit色情報クラス
 */
class RgbaColour
{
public:
  std::uint8_t red;   ///< 赤要素
  std::uint8_t green; ///< 緑要素
  std::uint8_t blue;  ///< 青要素
  std::uint8_t alpha; ///< アルファ値(不透過度)

public:
  /// @brief 構築子
  ///
  /// RGBA(0, 0, 0, 255)で初期化する。
  constexpr RgbaColour() noexcept : red(0), green(0), blue(0), alpha(255) {}

  /// @brief 構築子
  ///
  /// RGBA値を指定して初期化する。
  constexpr
  RgbaColour(
    std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
    : red(r), green(g), blue(b), alpha(a)
    {}

  /// @brief 構築子
  explicit constexpr RgbaColour(const RgbColour& rgb) noexcept
    : red(rgb.red), green(rgb.green), blue(rgb.blue), alpha(255)
    {}

  /// @brief 變換
  explicit operator RgbColour() const noexcept
    { return RgbColour(red, green, blue); }

  /// @brief 等價比較演算子
  constexpr
  bool operator==(const RgbaColour& other) const noexcept
  {
    return
      red == other.red && green == other.green
      && blue == other.blue && alpha == other.alpha;
  }

  /// @brief 非等價比較演算子
  constexpr
  bool operator!=(const RgbaColour& other) const noexcept
    { return !(*this == other); }
};




//// RGBとRGBAの比較: alpha値は無視して比較する ////////

constexpr
inline
bool operator==(const RgbaColour& x, const RgbColour& y) noexcept
  { return x.red == y.red && x.green == y.green && x.blue == y.blue; }

constexpr
inline
bool operator!=(const RgbaColour& x, const RgbColour& y) noexcept
  { return !(x == y); }

constexpr
inline
bool operator==(const RgbColour& x, const RgbaColour& y) noexcept
  { return y == x; }

constexpr
inline
bool operator!=(const RgbColour& x, const RgbaColour& y) noexcept
  { return !(x == y); }




//// ImageBuffer<>::bltで用ゐる轉寫用の函數オブジェクト ////////

/**
 * @brief ImageBuffer<>::blt()で用ゐる單純轉送のための函數オブジェクトクラス
 */
class NormalBrendCopier
{
public:
  void operator()(const RgbaColour& src, RgbColour& dst) noexcept
  {
    dst.red = src.red;
    dst.green = src.green;
    dst.blue = src.blue;
  }

  void operator()(const RgbColour& src, RgbaColour& dst) noexcept
  {
    dst.red = src.red;
    dst.green = src.green;
    dst.blue = src.blue;
    // dst.alphaは變更しない
  }
};


/**
 * @brief
 *   ImageBuffer<>::blt()で用ゐる加算ブレンドのための函數オブジェクトクラス
 */
class AddBrendCopier
{
public:
  void operator()(const RgbColour& src, RgbColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red + src.red, 0, 255);
    dst.green = std::clamp(dst.green + src.green, 0, 255);
    dst.red = std::clamp(dst.blue + src.blue, 0, 255);
  }

  void operator()(const RgbaColour& src, RgbColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red + src.red, 0, 255);
    dst.green = std::clamp(dst.green + src.green, 0, 255);
    dst.red = std::clamp(dst.blue + src.blue, 0, 255);
  }

  void operator()(const RgbColour& src, RgbaColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red + src.red, 0, 255);
    dst.green = std::clamp(dst.green + src.green, 0, 255);
    dst.red = std::clamp(dst.blue + src.blue, 0, 255);
    // dst.alphaは變更しない
  }

  void operator()(const RgbaColour& src, RgbaColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red + src.red, 0, 255);
    dst.green = std::clamp(dst.green + src.green, 0, 255);
    dst.red = std::clamp(dst.blue + src.blue, 0, 255);
    // dst.alphaは變更しない
  }
};


/**
 * @brief
 *   ImageBuffer<>::blt()で用ゐる乘算ブレンドのための函數オブジェクトクラス
 */
class MulBrendCopier
{
public:
  void operator()(const RgbColour& src, RgbColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red * src.red / 255, 0, 255);
    dst.green = std::clamp(dst.green * src.green / 255, 0, 255);
    dst.blue = std::clamp(dst.blue * src.blue / 255, 0, 255);
  }

  void operator()(const RgbaColour& src, RgbColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red * src.red / 255, 0, 255);
    dst.green = std::clamp(dst.green * src.green / 255, 0, 255);
    dst.blue = std::clamp(dst.blue * src.blue / 255, 0, 255);
  }

  void operator()(const RgbColour& src, RgbaColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red * src.red / 255, 0, 255);
    dst.green = std::clamp(dst.green * src.green / 255, 0, 255);
    dst.blue = std::clamp(dst.blue * src.blue / 255, 0, 255);
    // dst.alphaは變更しない
  }

  void operator()(const RgbaColour& src, RgbaColour& dst) noexcept
  {
    dst.red = std::clamp(dst.red * src.red / 255, 0, 255);
    dst.green = std::clamp(dst.green * src.green / 255, 0, 255);
    dst.blue = std::clamp(dst.blue * src.blue / 255, 0, 255);
    // dst.alphaは變更しない
  }
};


/**
 * @brief
 *   ImageBuffer<>::blt()で用ゐるαブレンドのための函數オブジェクトクラス
 */
class AlphaBrendCopier
{
public:
  void operator()(const RgbaColour& src, RgbColour& dst) noexcept
  {
    dst.red
      = std::clamp(
          src.red * src.alpha / 255 + dst.red * (255 - src.alpha) / 255,
          0, 255);
    dst.green
      = std::clamp(
          src.green * src.alpha / 255 + dst.green * (255 - src.alpha) / 255,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * src.alpha / 255 + dst.blue * (255 - src.alpha) / 255,
          0, 255);
  }

  void operator()(const RgbaColour& src, RgbaColour& dst) noexcept
  {
    dst.red
      = std::clamp(
          src.red * src.alpha / 255 + dst.red * (255 - src.alpha) / 255,
          0, 255);
    dst.green
      = std::clamp(
          src.green * src.alpha / 255 + dst.green * (255 - src.alpha) / 255,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * src.alpha / 255 + dst.blue * (255 - src.alpha) / 255,
          0, 255);
    dst.alpha
      = std::clamp(
          src.alpha + dst.alpha * (255 - src.alpha) / 255,
          0, 255);
  }
};


/// @brief ImageBuffer<>::blt()で用ゐるαブレンドのための函數オブジェクトクラス
///
/// このクラスのオブジェクトをImageBuffer<>::blt()で用ゐるとき、
/// (1) 轉送元の畫素の型がRgbColourである場合には、
/// いづれの畫素にも保持するα値を用ゐてαブレンドを行ひ、
/// (2) 轉送元の畫素の型がRgbaColourである場合には、
/// 保持するα値を各畫素のα値に乘じて、αブレンドを行ふ。
class CopierUsingFixedAlpha
{
private:
  std::uint8_t alpha_;

public:
  explicit CopierUsingFixedAlpha(std::uint8_t a) noexcept : alpha_(a) {}

  void operator()(const RgbColour& src, RgbColour& dst) const noexcept
  {
    dst.red
      = std::clamp(
          src.red * alpha_ / 255 + dst.red * (255 - alpha_) / 255,
          0, 255);
    dst.green
      = std::clamp(
          src.green * alpha_ / 255 + dst.green * (255 - alpha_) / 255,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * alpha_ / 255 + dst.blue * (255 - alpha_) / 255,
          0, 255);
  }

  void operator()(const RgbaColour& src, RgbColour& dst) const noexcept
  {
    // srcのアルファ値を、固定アルファ値alpha_で乘じた上で用ゐる。
    int a = alpha_ * src.alpha;
    dst.red
      = std::clamp(
          src.red * a / 65025 + dst.red * (65025 - a) / 65025,
          0, 255);
    dst.green
      = std::clamp(
          src.green * a / 65025 + dst.green * (65025 - a) / 65025,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * a / 65025 + dst.blue * (65025 - a) / 65025,
          0, 255);
  }

  void operator()(const RgbColour& src, RgbaColour& dst) const noexcept
  {
    dst.red
      = std::clamp(
          src.red * alpha_ / 255 + dst.red * (255 - alpha_) / 255,
          0, 255);
    dst.green
      = std::clamp(
          src.green * alpha_ / 255 + dst.green * (255 - alpha_) / 255,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * alpha_ / 255 + dst.blue * (255 - alpha_) / 255,
          0, 255);
    dst.alpha
      = std::clamp(
          alpha_ + dst.alpha * (255 - alpha_) / 255,
          0, 255);
  }

  void operator()(const RgbaColour& src, RgbaColour& dst) const noexcept
  {
    // srcのアルファ値を、固定アルファ値alpha_で乘じた上で用ゐる。
    int a = alpha_ * src.alpha;
    dst.red
      = std::clamp(
          src.red * a / 65025 + dst.red * (65025 - a) / 65025,
          0, 255);
    dst.green
      = std::clamp(
          src.green * a / 65025 + dst.green * (65025 - a) / 65025,
          0, 255);
    dst.blue
      = std::clamp(
          src.blue * a / 65025 + dst.blue * (65025 - a) / 65025,
          0, 255);

    dst.alpha
      = std::clamp(
          a / 255 + dst.alpha * (65025 - a) / 65025,
          0, 255);
  }
};


}//end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_COLOUR_H
