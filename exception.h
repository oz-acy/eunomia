/*
 * Copyright 2021 oZ/acy (名賀月晃嗣) <acy@hiemalis.org>
 *
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
 * @file exception.h
 * @author oZ/acy (名賀月晃嗣)
 * @brief 例外クラス
 *
 * @date R3.4.29 v0.1
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_EXCEPTION_H
#define INCLUDE_GUARD_EUNOMIA_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace eunomia
{
/**
 * @brief 例外
 *
 * std::runtime_errorを繼承し、定型のwhat_argを保持する例外クラス。
 */
class Exception : public std::runtime_error
{
public:
  /// @brief デフォルト構築子
  ///
  /// what_argに"Exception"を與へて初期化する。
  Exception() : runtime_error("eunomia::Exception") {}

  /// @brief 構築子
  ///
  /// what_argを與へて初期化する。
  ///
  /// @param m メッセージ文字列
  explicit Exception(const std::string& m) : runtime_error(m) {}

  /// @brief 構築子
  /// 
  /// what_argを「函數名(): メッセージ」の書式で初期化する。
  /// @param f 函數名
  /// @param m メッセージ
  Exception(const std::string& f, const std::string& m)
    : runtime_error(f + "(): " + m)
  {}

  /// @brief 構築子
  ///
  /// what_argを「クラス名::函數名(): メッセージ」の書式で初期化する。
  /// 
  /// @param c クラス名
  /// @param f 函數名
  /// @param m メッセージ
  Exception(const std::string& c, const std::string& f, const std::string& m)
    : runtime_error(c + "::" + f + "(): " + m)
  {}
};


}// end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_EXCEPT_H
