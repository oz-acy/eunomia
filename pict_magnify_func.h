/*
 * Copyright 2003-2021 oZ/acy (名賀月晃嗣)
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
 * @author oZ/acy (名賀月晃嗣)
 * @brief Picture::magnify()とPictureRgba::magnify()の共用函數の實裝
 *
 * @date 24 Apr MMXXI
 *   Picture::magnify()、PictureRgba::magnify()で共用する函數を摘出
 *
 */
#ifndef INCLUDE_GUARD_EUNOMIA_PICTURE_MAGNIFY_FUNCTION_H
#define INCLUDE_GUARD_EUNOMIA_PICTURE_MAGNIFY_FUNCTION_H


namespace eunomia::implement_
{

// aの追加はWikipedia:en:Bicubic interpolationに據る。(R3.4.23)
// 從前は a = -1.0 固定であつた。
inline
double fCubic_(double t, double a)
{
  if (t < 0)
    t = -t;

  if (t < 1.0)
    return 1.0 + t * t * ((a + 2.0) * t - a - 3.0);
    //return t * t * (t - 2.0) + 1.0;
  else if (t < 2.0)
    return a * (t * (t * (t - 5.0) + 8.0) - 4.0);
    //return -t * (t * (t - 5.0) + 8.0) + 4.0;
  else
    return 0.0;
}

inline
double innerProduct_(int n, double* x, double* y)
{
  double res = 0.0;
  for (int i=0; i < n; i++)
    res += x[i] * y[i];
  return res;
}

inline
void productMat14_44_(double* res, double* vec, double* mat)
{
  for (int i = 0; i < 4; i++) {
    res[i] = 0.0;
    for (int j = 0; j < 4; j++)
      res[i] += vec[j] * mat[i + j * 4];
  }
}


}// end of namespace eunomia::implement_




#endif // INCLUDE_GUARD_EUNOMIA_PICTURE_MAGNIFY_FUNCTION_H
