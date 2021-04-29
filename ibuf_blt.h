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
/*
 * @file ibuf_blt.h
 * @author oZ/acy
 * @brief 畫像バッファクラステンプレートのblt系メンバ函數の實裝
 *
 * @date 2021.4.22 作成
 *
 */
/* This file is included by "imagebuffer.h". */


namespace eunomia::implement_
{
/**
 * @brief クリッピング處理クラス
 */
class Clipper_
{
public:
  int sx, sy;
  int w, h;
  int dx, dy;

private:
  bool flag_;

public:
  /// @brief 構築子
  ///
  /// @param srcx 轉送元X座標
  /// @param srcy 轉送元Y座標
  /// @param srcw 轉送元畫像の幅
  /// @param srch 轉送元畫像の高さ
  /// @param bltw 轉送幅
  /// @param blth 轉送高さ
  /// @param dstx 轉送先X座標
  /// @param dsty 轉送先Y座標
  /// @param dstw 轉送先畫像の幅
  /// @param dsth 轉送先畫像の幅
  /// @param cliprect 轉送先の變更可能な長方形の領域
  Clipper_(
    int srcx, int srcy, int srcw, int srch, int bltw, int blth,
    int dstx, int dsty, int dstw, int dsth, 
    const std::optional<Rect>& cliprect);

  explicit operator bool() const { return flag_; }
};


}//end of namespace eunomia::implement_




template<class C_> template<class CSrc, class Copier>
inline
void
eunomia::ImageBuffer<C_>::blt(
  const eunomia::ImageBuffer<CSrc>& src, int sx, int sy, int w, int h,
  int dx, int dy, const std::optional<eunomia::Rect>& cliprect,
  Copier copier)
{
  implement_::Clipper_
    clipper(
      sx, sy, src.width(), src.height(), w, h,
      dx, dy, width(), height(), cliprect);

  if (clipper) {
    auto dl = buffer() + pitch() * clipper.dy;
    auto sl = src.buffer() + src.pitch() * clipper.sy;

    for (int j = 0; j < clipper.h; ++j, dl += pitch(), sl += src.pitch())
      for (int i = 0; i < clipper.w; ++i)
        copier(
          reinterpret_cast<const CSrc*>(sl)[clipper.sx + i],
          reinterpret_cast<C_*>(dl)[clipper.dx + i]);
  }
}




//eof
