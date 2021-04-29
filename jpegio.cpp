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
/*
 * @file jpegio.cpp
 * @author oZ/acy
 * @brief JPEG入出力のエラーハンドラの實裝
 *
 * @date 2021.4.28 LIBPOLYMNIAから改作
 *
 */
#include <iostream>
#include "jpegio_implement.h"
#include "debuglogger.h"


namespace
{

void myErrorOutput_(j_common_ptr cinfo)
{
  char buf[JMSG_LENGTH_MAX];
  cinfo->err->format_message(cinfo, buf);
  DLOG << DLTSTMP << buf << std::endl;
}

void myErrorExit_(j_common_ptr cinfo)
{
  cinfo->err->output_message(cinfo);
  throw eunomia::implement_::JpegIOException_();
}


}// end of NONAME namespace


void eunomia::implement_::jpegErrorSetup_(jpeg_error_mgr& jerr)
{
  jerr.error_exit = myErrorExit_;
  jerr.output_message = myErrorOutput_;
}




//eof
