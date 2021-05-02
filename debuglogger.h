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
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H
#define INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H

#include <filesystem>

#ifndef NDEBUG
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#endif // NDEBUG


#ifndef DLOG
/**
 * @brief デバッグログ出力ストリーム
 *
 * デバッグログを出力する出力ストリーム。DEBUGLOGGERと同じストリームを指す。
 */
#define DLOG eunomia::Debug::logger()
#endif
#ifndef DLTSTMP
/**
 *  @brief デバッグログ出力ストリームにタイムスタンプを出力するオブジェクト
 *
 * デバッグログ出力ストリームに插入子<<で與へると、
 * タイムスタンプを出力するオブジェクト。DEBUGLOGGER_TIMESTAMPと同じ。
 */
#define DLTSTMP eunomia::Debug::Timestamp()
#endif
#ifndef DEBUGLOGGER
/**
 * @brief デバッグログ出力ストリーム
 *
 * デバッグログを出力する出力ストリーム。DLOGと同じストリームを指す。
 */
#define DEBUGLOGGER eunomia::Debug::logger()
#endif
#ifndef DEBUGLOGGER_TIMESTAMP
/**
 * @brief デバッグログ出力ストリームにタイムスタンプを出力するオブジェクト
 *
 * デバッグログ出力ストリームに插入子<<で與へると、
 * タイムスタンプを出力するオブジェクト。DLTSTMPと同じ。
 */
#define DEBUGLOGGER_TIMESTAMP eunomia::Debug::Timestamp()
#endif


namespace eunomia
{

/**
 * @brief デバッグログ出力ストリーム
 *
 * デバッグログを出力するためのファイル出力ストリームを管理し、
 * ログを出力するための插入子<<を提供する。
 */
class Debug
{
public:
  /**
   * @brief タイムスタンプ
   *
   * Debug は、Timestamp のオブジェクトを插入(<<)されると、
   * その管理してゐるファイル出力ストリームにタイムスタンプを出力する。
   */
  class Timestamp{};


#ifdef NDEBUG
private:
  Debug(const std::filesystem::path& path) noexcept {}

public:
  template<typename T>
  Debug& operator<<(const T& x) { return *this; }

  template<typename T>
  Debug& operator<<(const T* x) { return *this; }

  Debug& operator<<(std::ostream& (*mnp)(std::ostream&)) { return *this; }
  Debug& operator<<(const Timestamp& ts) { return *this; }


#else
private:
  std::ofstream ofs_;

  Debug(const std::filesystem::path& path) : ofs_(path, std::ios::app) {}

public:
  template<typename T>
  Debug& operator<<(const T& x)
  {
    ofs_ << x;
    return *this;
  }

  template<typename T>
  Debug& operator<<(const T* x)
  {
    ofs_ << x;
    return *this;
  }

  Debug& operator<<(std::ostream& (*mnp)(std::ostream&))
  {
    ofs_ << mnp;
    return *this; 
  }

  Debug& operator<<(const Timestamp& ts)
  {
    auto now = std::time(nullptr);
    ofs_ << std::put_time(std::localtime(&now), "[%Y/%m/%d %H:%M:%S] ");
    return *this;
  }


#endif // NDEBUG

public:
  /**
   * @brief デバッグログ出力ストリームの取得
   *
   * 唯一のデバッグログ出力ストリームのインスタンスを取得する。
   */
  static Debug& logger()
  {
    static Debug dl("debuglog.txt");
    return dl;
  }
};


}//end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_DEBUG_LOGGER_H
