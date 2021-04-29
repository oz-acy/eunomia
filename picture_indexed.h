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
 *  @file picture_indexed.h
 *  @author oZ/acy
 *  @brief RGB24bit256インデックスの畫像バッファクラス
 *
 *  @date 2016.3.2 C++11對應(假)
 *  @date 2018.12.28 グレイスケール化、擴大、縮小函數をメンバ函數化
 *  @date 2019.8.29 createなどの返却型を生ポインタからunique_ptrに變更
 *  @date 2021.3.25 themis/inttype.hの非推奬化に對應
 *
 *  @date 2021.4.29 v0.1
 *    LIBPOLYMNIAのRGB24bit256インデックス畫像バッファクラスから改作
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXED_H
#define INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXED_H

#include <memory>
#include "imagebuffer.h"
#include "colour.h"


namespace eunomia
{
  class Picture;
  class PictureRgba;

/**
 * @brief RGB24bit256インデックスの畫像バッファ
 */
class PictureIndexed : public ImageBuffer<std::uint8_t>
{
private:
  /// @brief 畫像バッファとして確保した領域の資源管理のためのunique_ptr
  std::unique_ptr<std::uint8_t[]> upbuf_;

protected:
  RgbColour pal_[256]; ///< パレット

  /// @brief 構築子
  /// @param w 幅
  /// @param h 高さ
  PictureIndexed(unsigned w, unsigned h);

public:
  /// @brief 畫像バッファ生成
  ///
  /// 幅と高さを指定してPictureIndexedオブジェクトを生成する。
  /// @param w 幅
  /// @param h 高さ
  static
  std::unique_ptr<PictureIndexed> create(unsigned w, unsigned h) noexcept;

  /// @brief 複製
  std::unique_ptr<PictureIndexed> clone() const noexcept;

  /// @brief 複製(RGB24bit化)
  ///
  /// RGB24bit化した複製を生成する。
  std::unique_ptr<Picture> duplicatePicture() const noexcept;


  //// パレットへのアクセス ////////

  /// @brief パレットの參照
  /// @param id パレットID 0〜255
  /// @return IDで指定されるパレットへの參照
  eunomia::RgbColour& palette(int id) noexcept { return pal_[id]; }

  /// @brief パレットの參照
  /// @param id パレットID 0〜255
  /// @return IDで指定されるパレットへの參照
  const eunomia::RgbColour& palette(int id) const noexcept { return pal_[id]; }

  /// @brief パレットバッファの參照
  ///
  /// パレットのバッファへのポインタを取得する。
  eunomia::RgbColour* paletteBuffer() noexcept { return pal_; }

  /// @brief パレットバッファの參照
  ///
  /// パレットのバッファへのポインタを取得する。
  const eunomia::RgbColour* paletteBuffer() const noexcept { return pal_; }
};


/**
 * @brief PictureIndexed を單純に轉送するための函數オブジェクトクラス
 *
 * PictureIndexed を 
 * ImageBuffer<RgbColour>::blt() あるいは ImageBuffer<RgbaColour>::blt()
 * の轉送元として、單純な轉寫を行ふ。
 */
class NormalBrendCopierFromPictureIndexed
{
private:
  const PictureIndexed& pi_;

public:
  explicit
  NormalBrendCopierFromPictureIndexed(const PictureIndexed& p) noexcept
    : pi_(p)
    {}

  void operator()(std::uint8_t src, RgbColour& dst) const noexcept
  {
    dst = pi_.palette(src);
  }

  void operator()(std::uint8_t src, RgbaColour& dst) const noexcept
  {
    dst.red = pi_.palette(src).red;
    dst.green = pi_.palette(src).green;
    dst.blue = pi_.palette(src).blue;
  }
};




}// end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_PICTURE_INDEXED_H
