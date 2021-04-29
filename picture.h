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
 *  @file picture.h
 *  @author oZ/acy
 *  @brief RGB24bitの畫像バッファクラス
 *
 *  @date 2016.3.2  C++11對應(假)
 *  @date 2018.12.28  グレイスケール化、擴大、縮小函數をメンバ函數化
 *  @date 2019.8.29  createなどの返却型を生ポインタからunique_ptrに變更
 *  @date 2021.3.25  themis/inttype.hの非推奬化に對應
 *
 *  @date 2021.4.29 v0.1
 *    LIBPOLYMNIAのRGB24bit畫像バッファクラスから改作
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PICTURE_H
#define INCLUDE_GUARD_EUNOMIA_PICTURE_H

#include <memory>
#include "imagebuffer.h"
#include "colour.h"


namespace eunomia
{
  class PictureRgba;
  class PictureIndexed;


/**
 *  @brief RGB24bitの畫像バッファ
 */
class Picture : public ImageBuffer<RgbColour>
{
private:
  /// @brief 畫像バッファとして確保した領域の資源管理のためのunique_ptr
  std::unique_ptr<RgbColour[]> upbuf_;

protected:
  /// @brief 構築子
  /// @param w 畫像の幅
  /// @param h 畫像の高さ
  Picture(unsigned w, unsigned h);

public:
  /// @brief 畫像バッファ生成
  ///
  /// 幅と高さを指定してPictureオブジェクトを生成する。
  /// @param w 畫像の幅
  /// @param h 畫像の高さ
  static std::unique_ptr<Picture> create(unsigned w, unsigned h) noexcept;

  /// @brief 複製
  std::unique_ptr<Picture> clone() const noexcept;

  //// @brief グレイスケール化
  ////
  //// 自己の内容をグレイスケール化する。
  //void grayscale() noexcept;

  /// @brief グレイスケール化複製
  ///
  /// グレイスケール化した複製を生成する。
  std::unique_ptr<PictureIndexed>
  createGrayscaledPictureIndexed() const noexcept;

  /// @brief 減色(インデックスカラー化)
  ///
  /// 256インデックスカラー化した複製を生成する。
  std::unique_ptr<PictureIndexed> duplicatePictureIndexed() const noexcept;

  /// @brief 擴大
  ///
  /// 擴大した複製を生成する。
  ///
  /// @param w 複製畫像の幅
  /// @param h 複製畫像の高さ
  /// @param a 
  ///   シャープネスを加減するパラメタ。
  ///   Wikipedia:en:Bicubic interpolationによれば
  ///   -0.5〜-0.75がusuallyであるらしいが、
  ///   初めて觸れた參考文獻では-1.0に固定されてゐた。
  std::unique_ptr<Picture>
  magnify(int w, int h, double a = -1.0) const noexcept;

  /// @brief 縮小
  ///
  /// 縮小した複製を生成する。
  std::unique_ptr<Picture> reduce(int w, int h) const noexcept;
};


}// end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_PICTURE_H
