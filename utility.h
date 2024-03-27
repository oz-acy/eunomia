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
 * @date R3.11.23 lower()の追加
 * @date R3.12.10 必要なヘッダのincludeの追加
 * @date R6.3.28 toLower(const std::wstring&)をlower()に改名
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_UTILITY_H
#define INCLUDE_GUARD_EUNOMIA_UTILITY_H

#include <cctype>
#include <cwctype>
#include <string>
#include <vector>

namespace eunomia
{

//////// LOWER STRING ////////////////
/**
 * @brief 小文字文字列化
 *
 * 文字列str中の大文字を小文字に變換した文字列を返す。
 * @param str 對象文字列
 * @return 變換結果
 */
inline
std::string lower(const std::string& str)
{
  std::string res = str;
  for (auto& c : res) {
    c = std::tolower((unsigned char)c);
  }
  return res;
}

/**
 * @brief 小文字文字列化
 *
 * 文字列str中の大文字を小文字に變換した文字列を返す。
 * @param str 對象文字列
 * @return 變換結果
 */
inline
std::wstring lower(const std::wstring& str)
{
  std::wstring res = str;
  for (auto& c : res) {
    c = std::towlower(c);
  }
  return res;
}


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


//////// STRING TRIM ////////////////
/**
 * @brief 文字列の先頭と末尾からの指定文字の除去
 *
 * 文字列の先頭と末尾から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの先頭と末尾から除去されるべき文字を列擧した文字列
 * @return strの先頭と末尾からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
trim(
  const std::basic_string<C, T, A>& str,
  const std::basic_string<C, T, A>& removed)
{
  auto l = str.find_first_not_of(removed);

  if (l == std::basic_string<C, T, A>::npos)
    return std::string("");

  auto r = str.find_last_not_of(removed);
  return str.substr(l, r - l + 1);
}


/**
 * @brief 文字列の先頭と末尾からの指定文字の除去
 *
 * 文字列の先頭と末尾から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの先頭と末尾から除去されるべき文字を列擧したC文字列
 * @return strの先頭と末尾からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
trim(const std::basic_string<C, T, A>& str, const C* removed)
{
  auto l = str.find_first_not_of(removed);

  if (l == std::basic_string<C, T, A>::npos)
    return std::string("");

  auto r = str.find_last_not_of(removed);
  return str.substr(l, r - l + 1);
}


/**
 * @brief 文字列の先頭からの指定文字の除去
 *
 * 文字列の先頭から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの先頭から除去されるべき文字を列擧した文字列
 * @return strの先頭からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
ltrim(
  const std::basic_string<C, T, A>& str,
  const std::basic_string<C, T, A>& removed)
{
  auto l = str.find_first_not_of(removed);

  if (l == std::basic_string<C, T, A>::npos)
    return std::string("");
  else
    return str.substr(l, str.size() - l);
}


/**
 * @brief 文字列の先頭からの指定文字の除去
 *
 * 文字列の先頭から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの先頭から除去されるべき文字を列擧したC文字列
 * @return strの先頭からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
ltrim(const std::basic_string<C, T, A>& str, const C* removed)
{
  auto l = str.find_first_not_of(removed);

  if (l == std::basic_string<C, T, A>::npos)
    return std::string("");
  else
    return str.substr(l, str.size() - l);
}


/**
 * @brief 文字列の末尾からの指定文字の除去
 *
 * 文字列の末尾から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの末尾から除去されるべき文字を列擧した文字列
 * @return strの末尾からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
rtrim(
  const std::basic_string<C, T, A>& str,
  const std::basic_string<C, T, A>& removed)
{
  auto r = str.find_last_not_of(removed);
  if (r == std::basic_string<C, T, A>::npos)
    return std::string("");
  else
    return str.substr(0, r + 1);
}


/**
 * @brief 文字列の末尾からの指定文字の除去
 *
 * 文字列の末尾から、指定された文字を除去した文字列を返す。
 * @param str 操作の對象となる文字列
 * @param removed strの末尾から除去されるべき文字を列擧したC文字列
 * @return strの末尾からremovedを構成する文字を除去した文字列
 */
template<class C, class T, class A>
inline
std::basic_string<C, T, A>
rtrim(const std::basic_string<C, T, A>& str, const C* removed)
{
  auto r = str.find_last_not_of(removed);
  if (r == std::basic_string<C, T, A>::npos)
    return std::string("");
  else
    return str.substr(0, r + 1);
}





}// end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_UTILITY_H
