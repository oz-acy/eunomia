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
 * @file ibuf_blt.cpp
 * @author oZ/acy
 * @brief 畫像バッファ轉送のクリッピングの實裝
 *
 * @date 2021.4.22 作成
 *
 */
#include "imagebuffer.h"

eunomia::implement_::Clipper_::Clipper_(
  int srcx, int srcy, int srcw, int srch, int bltw, int blth,
  int dstx, int dsty, int dstw, int dsth, const std::optional<Rect>& cliprect)
  : flag_(false)
{
  // 轉送元のクリッピング
  int sleft = std::max(srcx, 0);
  int sright = std::min(srcx + bltw, srcw);
  int stop = std::max(srcy, 0);
  int sbottom = std::min(srcy + blth, srch);

  if (sleft > sright || stop > sbottom)
    return;

  // 轉送元のクリッピングを受けて、轉送先座標を(必要なら)右下方向にずらす。
  dstx += sleft - srcx;
  dsty += stop - srcy;

  // 轉送元のクリッピングを受けて、轉送する幅と高さを更新する。
  bltw = sright - sleft;
  blth = sbottom - stop;

  // 轉送先のクリッピング
  int dleft, dtop, dright, dbottom;
  if (cliprect) {
    dleft = std::max({dstx, cliprect->left, 0});
    dtop = std::max({dsty, cliprect->top, 0});
    dright = std::min({dstx + bltw, dstw, cliprect->right});
    dbottom = std::min({dsty + blth, dsth, cliprect->bottom});
  } else {
    dleft = std::max(dstx, 0);
    dtop = std::max(dsty, 0);
    dright = std::min(dstx + bltw, dstw);
    dbottom = std::min(dsty + blth, dsth);
  }

  if (dleft > dright || dtop > dbottom)
    return;

  dx = dleft;
  dy = dtop;
  w = dright - dleft;
  h = dbottom - dtop;

  sx = sleft + dleft - dstx;
  sy = stop + dtop - dsty;

  flag_ = true;
}


//eof
