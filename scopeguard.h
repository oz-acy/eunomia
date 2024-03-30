/*
 * Copyright 2024 oZ/acy (名賀月晃嗣) <acy@hiemalis.org>
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
 * @file scopeguard.h
 * @author oZ/acy (名賀月晃嗣)
 * @brief スコープガード
 *
 * @date R6.3.30 作成開始
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_SCOPE_GUARD_H
#define INCLUDE_GUARD_EUNOMIA_SCOPE_GUARD_H

#include <utility>


namespace eunomia
{
template <typename Func> class ScopeGuard;

template <typename Func>
ScopeGuard<Func> makeScopeGuard(Func&&);

/**
 * @brief スコープガードクラステンプレート
 */
template <typename Func>
class ScopeGuard
{
private:
  Func func_;
  bool active_;

  ScopeGuard() = delete;
  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;
  // ムーヴ構築子、ムーヴ代入は、
  // デストラクタのユーザ宣言により暗默的には提供されないので、deleteするまでもない。

  explicit ScopeGuard(Func&& f) : func_(std::forward<Func>(f)), active_(true) {}

public:
  ~ScopeGuard()
  {
    if (active_)
      func_();
  }

  /// @brief 解任
  ///
  /// スコープガードの役割を解く。
  /// 例へばrollbackが必要なくなつた場合などに用ゐる。
  void dismiss() noexcept { active_ = false; }

  friend ScopeGuard<Func> makeScopeGuard<Func>(Func&&);
};


/**
 * @brief スコープガード作成
 * @param f コールバックされる函數(オブジェクト)
 */
template <typename Func>
inline
ScopeGuard<Func> makeScopeGuard(Func&& f)
{
  return ScopeGuard<Func>(std::forward<Func>(f));
}


}// end of namespace eunomia


#endif // INCLUDE_GUARD_EUNOMIA_SCOPE_GUARD_H
