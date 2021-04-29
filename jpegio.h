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
 *  @file jpegio.h
 *  @author oZ/acy (名賀月晃嗣)
 *  @brief JPEG畫像の入出力
 *
 *  @date 2021.4.29 v0.1
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_JPEG_INPUT_OUTPUT_H
#define INCLUDE_GUARD_EUNOMIA_JPEG_INPUT_OUTPUT_H

#include <filesystem>
#include "picture.h"


namespace eunomia
{
/**
 * @brief JPEGファイルの讀み込み
 *
 * JPEGファイルを讀み込む。
 *
 * @param path 讀み込むべきPNGファイルのパス
 * @return
 *   讀み込みが成功した場合は生成されたPictureを管理するunique_ptrを返す。
 *   讀み込みが失敗した場合は管理する對象のないunique_ptrを返す。
 */
std::unique_ptr<Picture> loadJpeg(const std::filesystem::path& path) noexcept;

/**
 * @brief JPEGファイルの保存
 *
 * 畫像をJPEG形式で保存する。
 * @param pict 保存する畫像
 * @param path 保存すべきJPEGファイルのパス
 * @param prog プログレッシブ形式にするならtrue、さもなくばfalseを指定する。
 * @param quality 品質。0〜100で指定する。
 * @return 成功時にはtrue、さもなくばfalseを返す。
 */
bool
saveJpeg(
  const Picture& pict, const std::filesystem::path& path,
  bool prog = false, int quality = 75) noexcept;


}// end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_JPEG_INPUT_OUTPUT_H
