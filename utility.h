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
 * @file utility.h
 * @author oZ/acy (名賀月晃嗣)
 * @brief ユーティリティー
 *
 * @date R3.4.29 v0.1
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_UTILITY_H
#define INCLUDE_GUARD_EUNOMIA_UTILITY_H

#include <string>
#include <vector>

namespace eunomia
{

//////// STRING SPLIT ////////////////
/**
 * @brief 文字列分割
 *
 * 文字列strを、文字列dlmで指定するデリミタで分割する。
 * @param str 對象文字列
 * @param dlm デリミタとなる文字を列擧した文字列
 * @return 分割結果を格納した文字列のベクタ
 */
template<class C, class T, class A>
inline
std::vector<std::basic_string<C, T, A>>
split(
  const std::basic_string<C, T, A>& str, const std::basic_string<C, T, A>& dlm)
{
  std::vector<std::basic_string<C, T, A>> rvec;
  typename std::basic_string<C, T, A>::size_type f = 0;

  while (f < str.size()) {
    auto b = str.find_first_of(dlm, f);
    if (b == std::basic_string<C, T, A>::npos) {
      b = str.size();
    }
    if (f < b) {
      auto s = str.substr(f, b - f);
      rvec.push_back(s);
    }
    f = b + 1;
  }

  return rvec;
}


/**
 * @brief 文字列分割
 *
 * 文字列strを、C文字列dlmで指定するデリミタで分割する。
 * @param str 對象文字列
 * @param dlm デリミタとなる文字を列擧したC文字列
 * @return 分割結果を格納した文字列のベクタ
 */
template<class C, class T, class A>
inline
std::vector<std::basic_string<C, T, A>>
split(const std::basic_string<C, T, A>& str, const C* dlm)
{
  std::vector<std::basic_string<C, T, A>> rvec;
  typename std::basic_string<C, T, A>::size_type f = 0;

  while (f < str.size()) {
    auto b = str.find_first_of(dlm, f);
    if (b == std::basic_string<C, T, A>::npos) {
      b = str.size();
    }
    if (f < b) {
      auto s = str.substr(f, b - f);
      rvec.push_back(s);
    }
    f = b + 1;
  }

  return rvec;
}


/**
 * @brief 文字列分割
 *
 * 文字列strを、文字dlmで指定するデリミタで分割する。
 * @param str 對象文字列
 * @param dlm デリミタとなる文字
 * @return 分割結果を格納した文字列のベクタ
 */
template<class C, class T, class A>
inline
std::vector<std::basic_string<C, T, A>>
split(const std::basic_string<C, T, A>& str, C dlm)
{
  std::vector<std::basic_string<C, T, A>> rvec;
  typename std::basic_string<C, T, A>::size_type f = 0;

  while (f < str.size()) {
    auto b = str.find_first_of(dlm, f);
    if (b == std::basic_string<C, T, A>::npos) {
      b = str.size();
    }
    if (f < b) {
      auto s = str.substr(f, b - f);
      rvec.push_back(s);
    }
    f = b + 1;
  }

  return rvec;
}


}// end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_UTILITY_H