/*
 * Copyright 2021 oZ/acy (名賀月晃嗣)
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
 * @file debuglogger.h
 * @author oZ/acy(名賀月晃嗣)
 * @brief デバッグ用ロガー
 *
 * @date 2021.4.29 v0.1
 * @date 2021.5.5 仕樣を全面的に變更
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H
#define INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <ctime>


namespace eunomia::debug
{

namespace implement_
{
/**
 * @brief デバッグログ出力ストリーム
 *
 * デバッグ用のログを出力するためのファイル出力ストリームを管理し、
 * ログを出力するための插入子<<を提供する。
 */
class DebugLogger_
{
public:
  class Timestamp_{};

private:
  DebugLogger_() = default;

#ifdef NDEBUG

public:
  template<typename T>
  DebugLogger_& operator<<(const T& x) { return *this; }

  template<typename T>
  DebugLogger_& operator<<(const T* x) { return *this; }

  DebugLogger_& operator<<(std::ostream& (*mnp)(std::ostream&))
    { return *this; }

  DebugLogger_& operator<<(const Timestamp_& ts) { return *this; }

  static DebugLogger_& out()
  {
    static DebugLogger_ d;
    return d;
  }

  static void set(const std::filesystem::path& path) {}


#else // ifndef NDEBUG

private:
  static inline std::filesystem::path path_ = "eunomiaDebugLog.txt";
  static inline std::ofstream ofs_;

public:
  template<typename T>
  DebugLogger_& operator<<(const T& x)
  {
    ofs_ << x;
    return *this;
  }

  template<typename T>
  DebugLogger_& operator<<(const T* x)
  {
    ofs_ << x;
    return *this;
  }

  DebugLogger_& operator<<(std::ostream& (*mnp)(std::ostream&))
  {
    ofs_ << mnp;
    return *this; 
  }

  DebugLogger_& operator<<(const Timestamp_& ts)
  {
    auto now = std::time(nullptr);
    ofs_ << std::put_time(std::localtime(&now), "[%Y/%m/%d %H:%M:%S] ");
    return *this;
  }

  static DebugLogger_& out()
  {
    static DebugLogger_ d;
    if (!ofs_.is_open())
      ofs_.open(path_, std::ios::app);
    return d;
  }

  static void set(const std::filesystem::path& path) { path_ = path; }
#endif
};


}//endo of namespace implement_


inline implement_::DebugLogger_& out()
{
  return implement_::DebugLogger_::out();
}


inline implement_::DebugLogger_::Timestamp_ timestamp()
{
  return implement_::DebugLogger_::Timestamp_();
}


inline void setLoggingFile(const std::filesystem::path& path)
{
  implement_::DebugLogger_::set(path);
}


}//end of namespace eunomia::debug




#endif // INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H
