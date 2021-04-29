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
 * @file picture_rgba.h
 * @author oZ/acy (名賀月晃嗣)
 * @brief RGBA32bitの畫像バッファクラス
 * 
 * @date R3.4.29 v0.1
 *   RGB24bit畫像バッファクラスから改作
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PICTURE_RGBA_H
#define INCLUDE_GUARD_EUNOMIA_PICTURE_RGBA_H

#include <memory>
#include "imagebuffer.h"
#include "colour.h"


namespace eunomia
{
  class Picture;
  class PictureIndexed;


/**
 *  @brief RGBA32bitの畫像バッファ
 */
class PictureRgba : public ImageBuffer<RgbaColour>
{
private:
  /// @brief 畫像バッファとして確保した領域の資源管理のためのunique_ptr
  std::unique_ptr<RgbaColour[]> upbuf_;

protected:
  /// @brief 構築子
  /// @param w 幅
  /// @param h 高さ
  PictureRgba(unsigned w, unsigned h);

public:
  /// @brief 畫像バッファ生成
  static std::unique_ptr<PictureRgba> create(unsigned w, unsigned h) noexcept;

  /// @brief 複製
  std::unique_ptr<PictureRgba> clone() const noexcept;

  /// @brief αチャネル除去
  ///
  /// αチャネルを除いたPictureを生成する。
  std::unique_ptr<Picture> stripAlpha() const noexcept;

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
  std::unique_ptr<PictureRgba>
  magnify(int w, int h, double a = -1.0) const noexcept;

  /// @brief 縮小
  ///
  /// 縮小した複製を生成する。
  std::unique_ptr<PictureRgba> reduce(int w, int h) const noexcept;
};


}// end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_PICTURE_RGBA_H
