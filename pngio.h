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
 *  @file pngio.h
 *  @author oZ/acy
 *  @brief PNG畫像の入出力
 *
 *  @date 2021.4.29 v0.1
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PNG_INPUT_OUTPUT_H
#define INCLUDE_GUARD_EUNOMIA_PNG_INPUT_OUTPUT_H

#include <filesystem>
#include "picture.h"
#include "picture_rgba.h"
#include "picture_indexed.h"


namespace eunomia
{

/**
 * @brief PNGファイルの讀み込み
 *
 * PNGファイルを讀み込み、Picture, PictureRgba, PictureIndexed
 * のいづれかを生成し、ファイル内容を反映させる。
 *
 * 讀み込みが成功したときには、
 * 三つのunique_ptrのうち一つに實際に生成したオブジェクトが設定され、
 * 殘る二者にはnullptrが設定される。
 *
 * 讀み込みが失敗したときには、三つのunique_ptrにはnullptrが設定される。
 *
 * @param path 讀み込むべきPNGファイルのパス
 * @param[out] uppict 生成したPictureを管理するunique_ptr
 * @param[out] uprgba 生成したPictureRgbaを管理するunique_ptr
 * @param[out] upidx 生成したPictureIndexedを管理するunique_ptr
 * @return 讀み込みが成功した場合はtrue、さもなくばfalseを返す。
 */
bool loadPng(
  const std::filesystem::path& path,
  std::unique_ptr<Picture>& uppict,
  std::unique_ptr<PictureRgba>& uprgba,
  std::unique_ptr<PictureIndexed>& upindx);


/**
 * @brief PNGファイルの保存
 *
 * 畫像をPNG形式で保存する。
 * @param path 保存すべきPNGファイルのパス
 * @param pict 保存する畫像
 * @return 成功時にはtrue、さもなくばfalseを返す。
 */
bool savePng(const Picture& pict, const std::filesystem::path& path);

/**
 * @brief PNGファイルの保存
 *
 * 畫像をPNG形式で保存する。
 * @param path 保存すべきPNGファイルのパス
 * @param pict 保存する畫像
 * @return 成功時にはtrue、さもなくばfalseを返す。
 */
bool savePng(const PictureRgba& pict, const std::filesystem::path& path);

/**
 * @brief PNGファイルの保存
 *
 * 畫像をPNG形式で保存する。
 * @param path 保存すべきPNGファイルのパス
 * @param pict 保存する畫像
 * @return 成功時にはtrue、さもなくばfalseを返す。
 */
bool savePng(const PictureIndexed& pict, const std::filesystem::path& path);


}// end of namespace eunomia




#endif // INCLUDE_GUARD_EUNOMIA_PNG_INPUT_OUTPUT_H
