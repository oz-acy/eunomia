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
/**
 *  @file imagebuffer.h
 *  @author oZ/acy (名賀月晃嗣)
 *  @brief 畫像バッファクラステンプレート
 *
 *  @date 2016.3.2  C++11/14對應(假)
 *  @date 2018.12.21  C++17對應(假)
 *  @date 2019.8.26 boost::noncopyableをthemis::Noncopyable<>に差し替へ
 *
 *  @date 2021.4.29 v0.1
 *    LIBPOLYMNIAの畫像バッファクラステンプレートから改作
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_IMAGEBUFFER_H
#define INCLUDE_GUARD_EUNOMIA_IMAGEBUFFER_H

#include <algorithm>
#include <optional>
#include <cstdint>
#include "exception.h"
#include "noncopyable.h"
#include "rect.h"


namespace eunomia
{

/**
 * @brief 點
 */
struct Point
{
  int x; ///< X座標
  int y; ///< Y座標

  Point() = default;
  constexpr Point(int xx, int yy) noexcept : x(xx), y(yy) {}
};


/**
 * @brief 範圍逸脱例外
 */
class RangeOverException : public eunomia::Exception
{
public:
  RangeOverException() : Exception("eunomia::RangeOverException") {}
};




/**
 * @brief 畫像バッファ基底クラステンプレート
 */
template<class C_>
class ImageBuffer : Noncopyable<ImageBuffer<C_>>
{
public:
  typedef C_ ColourType;

protected:
  std::uint8_t* buf_;  ///< 畫像バッファ
  int w_;  ///< 幅
  int h_;  ///< 高さ
  int pitch_;  ///< ピッチ = 水平方向1ラインのビット數

  /// @brief 構築子
  ///
  /// 畫像バッファの幅、高さ、ピッチを指定値で初期化する。
  /// バッファそのものは派生クラスで確保される。
  /// @param w 幅
  /// @param h 高さ
  /// @param p ピッチ
  ImageBuffer(int w, int h, int p) noexcept
    : buf_(nullptr), w_(w), h_(h), pitch_(p)
    {}

public:
  /// 解體子
  virtual ~ImageBuffer() = default;

  //====================================
  //  各種情報取得
  //====================================

  /// @brief 幅
  int width() const noexcept { return w_; }
  /// @brief 高さ
  int height() const noexcept { return h_; }
  /// @brief ピッチ
  int pitch() const noexcept { return pitch_; }

  /// @brief バッファの先頭アドレスの取得
  uint8_t* buffer() noexcept { return buf_; }
  /// @brief バッファの先頭アドレスの取得
  const uint8_t* buffer() const noexcept { return buf_; }

  /// @brief ラインバッファの先頭アドレスの取得
  C_* lineBuffer(int y) noexcept
    { return reinterpret_cast<C_*>(buf_ + pitch_ * y); }

  /// @brief ラインバッファの先頭アドレスの取得
  const C_* lineBuffer(int y) const noexcept
    { return reinterpret_cast<const C_*>(buf_ + pitch_ * y); }

  //====================================
  //  畫素の參照
  //====================================

  /// @brief 畫素(x, y)の參照
  ///
  /// 座標(x, y)の畫素を參照する。
  C_& pixel(int x, int y) noexcept { return lineBuffer(y)[x]; }

  /// @brief 畫素(x, y)の參照
  ///
  /// 座標(x, y)の畫素を參照する。
  const C_& pixel(int x, int y) const noexcept { return lineBuffer(y)[x]; }

  /// @brief 畫素(x, y)の參照
  ///
  /// 座標(x, y)の畫素を參照する。
  /// 但し、座標(x, y)が畫像の幅と高さの範圍にあるか確認し、
  /// 範圍外の場合はRangeOverExceptionを投げる。
  C_& at(int x, int y)
  {
    if (x < 0 || y < 0 || x >= w_ || y >= h_)
      throw RangeOverException();
    return pixel(x, y);
  }

  /// @brief 畫素(x, y)の參照
  ///
  /// 座標(x, y)の畫素を參照する。
  /// 但し、座標(x, y)が畫像の幅と高さの範圍にあるか確認し、
  /// 範圍外の場合はRangeOverExceptionを投げる。
  const C_& at(int x, int y) const
  {
    if (x < 0 || y < 0 || x >= w_ || y >= h_)
      throw RangeOverException();
    return pixel(x, y);
  }


  //==================================================================
  //  圖形描畫
  //==================================================================

  /// @brief 線分の描畫
  /// @param x1 一方の端點のX座標
  /// @param y1 一方の端點のY座標
  /// @param x2 もう一方の端點のX座標
  /// @param y2 もう一方の端點のY座標
  /// @param color 色
  void line(int x1, int y1, int x2, int y2, const C_& color);

  /// @brief 長方形の描畫
  ///
  /// @param left 左邊のX座標
  /// @param top 上邊のY座標
  /// @param right 右邊のX座標
  /// @param bottom 下邊のY座標
  /// @param color 色
  /// @param fill 塗り潰すならtrue、さもなくばfalse
  void box(
    int left, int top, int right, int bottom, const C_& color,
    bool fill = false);

  /// @brief 楕圓の描畫
  /// @param x 中心のX座標
  /// @param y 中心のY座標
  /// @param a 水平方向の半徑
  /// @param b 垂直方向の半徑
  /// @param color 色
  /// @param fill 塗り潰すならtrue、さもなくばfalse
  void ellipse(
    int x, int y, int a, int b, const C_& color, bool fill = false);

  /// @brief 圓の描畫
  /// @param x 中心のX座標
  /// @param y 中心のY座標
  /// @param r 半徑
  /// @param color 色
  /// @param fill 塗り潰すならtrue、さもなくばfalse
  void circle(int x, int y, int r, const C_& color, bool fill = false)
  {
    ellipse(x, y, r, r, color, fill);
  }

  /// @brief 塗り潰し
  /// @param x 始點のX座標
  /// @param y 始點のY座標
  /// @param color 色
  void paintFill(int x, int y, const C_& color);

  /// @brief バッファ全體の塗り潰し
  /// @param color 色
  void clear(const C_& color);


  //==================================================================
  //  轉送
  //==================================================================

  /// @brief 轉送
  ///
  /// 轉送元畫像srcのsx, sy, w, hで指定される範圍から畫素を得て、
  /// 對應する畫素にcopierとして與へられる處理を行ふ。
  ///
  /// 各種ブレンディングを伴ふ轉送處理の實現を意圖した仕樣であるが、
  /// 他の用途があるかもしれない。
  ///
  /// @param src 轉送元畫像バッファ
  /// @param sx 轉送元左上X座標
  /// @param sy 轉送元左上Y座標
  /// @param w 轉送幅
  /// @param h 轉送高さ
  /// @param dx 左上X座標
  /// @param dy 左上Y座標
  /// @param cliprect 變更を許す長方形領域。std::nulloptの場合は畫像全體。
  /// @param copier
  ///   ピクセル毎の轉寫を行ふ函數あるいは函數オブジェクト。
  ///   轉送元(src)の畫素 const CSrc& s を
  ///   轉送先(*this)の畫素 C_& d に轉寫するときに、
  ///   copier(s, d)の形で呼び出される。
  template<class CSrc, class Copier>
  void
  blt(
    const ImageBuffer<CSrc>& src, int sx, int sy, int w, int h,
    int dx, int dy, const std::optional<Rect>& cliprect, Copier copier);

  /// @brief 轉送
  ///
  /// 畫素を轉送元の畫素で置き換へる。
  /// この函數を用ゐるには、CSrcからC_に代入が可能でなければならない。
  ///
  /// @param src 轉送元バッファ
  /// @param sx 轉送元左上X座標
  /// @param sy 轉送元左上Y座標
  /// @param w 轉送幅
  /// @param h 轉送高さ
  /// @param dx 左上X座標
  /// @param dy 左上Y座標
  /// @param cliprect 變更を許す長方形領域。std::nulloptの場合は畫像全體。
  template<class CSrc>
  void
  blt(
    const ImageBuffer<CSrc>& src, int sx, int sy, int w, int h,
    int dx, int dy, const std::optional<Rect>& cliprect = std::nullopt)
  {
    blt(
      src, sx, sy, w, h, dx, dy, cliprect,
      [](const CSrc& spixel, C_& dpixel){ dpixel = spixel; });
  }


  //==============================================
  // その他
  //==============================================

  /// @brief 畫素毎の處理
  ///
  /// 畫像の各畫素にfuncとして與へられる處理を行ふ。
  /// @param func 
  ///   各畫素への處理を行ふ函數あるいは函數オブジェクト。
  ///   畫素 C_& p を處理するときに、
  ///   func(p) の形で呼び出される。
  template<class Func>
  void forEachPixel(Func func)
  {
    for (int j = 0; j < h_; ++j)
      std::for_each_n(lineBuffer(j), w_, func);
  }
};


/// @brief グレイスケール化
///
/// ImageBuffer<C_>::forEachPixel()に與へると畫像をグレイスケール化する。
///
/// 前提は、C_がstd::uint8_tの公開メンバ變數red, green, blueを持つこと。
/// C_ = RgbColour or RgbaColour で用ゐることを意圖してゐる。
template<class C_>
void grayscale(C_& pixel)
{
  std::uint8_t l
    = pixel.red * 0.2990 + pixel.green * 0.5870 + pixel.blue * 0.1140;

  pixel.red = l;
  pixel.green = l;
  pixel.blue = l;
}


}//end of namespace eunomia


#include "ibuf_blt.h"
#include "ibuf_draw.h"


#endif // INCLUDE_GUARD_EUNOMIA_IMAGEBUFFER_H
