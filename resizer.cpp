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
 * @file resizer.cpp
 * @author oZ/acy
 * @brief 畫像擴大縮小プログラム
 *
 * @date R3.4.26 LIBPOLYMNIAからLIBEUNOMIAに移植するに伴ひ改作
 * @date R3.11.23 擴張子の小文字化處理をutility.hに切り出し
 *
 */
#include <iostream>
#include <filesystem>
#include <cctype>
#include <cwctype>
#include "dibio.h"
#include "pngio.h"
#include "jpegio.h"
#include "utility.h"


/**
 * @brief 大文字を小文字に變換した文字列の生成
 */
//std::string toLower(const std::string& str)
//{
//  std::string res = str;
//  for (auto& c : res) {
//    c = std::tolower((unsigned char)c);
//  }
//  return res;
//}

/**
 * @brief 大文字を小文字に變換した文字列の生成
 */
//std::wstring toLower(const std::wstring& str)
//{
//  std::wstring res = str;
//  for (auto& c : res) {
//    c = std::towlower(c);
//  }
//  return res;
//}




/**
 * @brief (擴張子に應じた)畫像の讀み込み
 */
bool
loadImage(
  const std::filesystem::path& path,
  std::unique_ptr<eunomia::Picture>& uppict,
  std::unique_ptr<eunomia::PictureRgba>& uprgba,
  std::unique_ptr<eunomia::PictureIndexed>& upindx)
{
  //auto e = toLower(path.extension().string());
  auto e = eunomia::lower(path.extension().string());

  if (e == ".bmp") {
    return eunomia::loadDib(path, uppict, upindx);
  }
  else if (e == ".png") {
    return eunomia::loadPng(path, uppict, uprgba, upindx);
  }
  else if (e == ".jpg" || e == ".jpeg") {
    uppict = eunomia::loadJpeg(path);
    return (bool)uppict;
  }
  else
    return false;
}


/**
 * @brief (擴張子に應じた)Pictureの保存
 */
bool saveImage(const eunomia::Picture& pict, const std::filesystem::path& path)
{
  //auto e = toLower(path.extension().string());
  auto e = eunomia::lower(path.extension().string());

  if (e == ".png") {
    return eunomia::savePng(pict, path);
  }
  else if (e == ".jpg" || e == ".jpeg") {
    return eunomia::saveJpeg(pict, path);
  }
  else if (e == ".bmp") {
    return eunomia::saveDib(pict, path);
  }
  else
    return false;
}


/**
 * @brief (擴張子に應じた)PictureRgbaの保存
 */
bool
saveImage(const eunomia::PictureRgba& pict, const std::filesystem::path& path)
{
  using fspath = std::filesystem::path;

  //auto ext = path.extension();
  //auto e = toLower(ext);
  auto e = eunomia::lower(path.extension().string());

  if (e == fspath(".png")) {
    return eunomia::savePng(pict, path);
  }
  //else if (ext == "jpg" || ext == "jpeg") {
  //  JpegSaver jsave;
  //  jsave.save(pict, file);
  //}
  //else { // if (ext=="bmp") {
  //  DibSaver bsave;
  //  bsave.save(pict, file);
  //}
  else
    return false;
}




//
//////// main ////////////////
//

int main(int argc, char** argv)
{
  bool valid = false;
  bool isP;  // true : -p power / false : -l width height
  int w = 0;
  int h = 0;
  double r;
  std::filesystem::path src, dst;


  if (argc == 5 && std::string(argv[1]) == "-p") {
    r = std::atof(argv[2]);
    if (r <= 0) {
      std::cerr << "指定された擴大率が不正。" << std::endl;
      return 1;
    }
    isP = true;
    src = argv[3];
    dst = argv[4];
    valid = true;  
  }
  else if (argc == 6 && std::string(argv[1]) == "-l") {
    w = std::atoi(argv[2]);
    h = std::atoi(argv[3]);
    if (w <= 0 || h <= 0) {
      std::cerr << "指定された幅あるいは高さが不正。" << std::endl;
      return 1;
    }
    isP = false;
    src = argv[4];
    dst = argv[5];
    valid = true;
  }


  if (!valid) {
    std::cerr << "usage: resizer -p power infile outfile\n";
    std::cerr << "       resizer -l width height infile outfile\n";
    return 1;
  }


  std::unique_ptr<eunomia::Picture> uppict;
  std::unique_ptr<eunomia::PictureRgba> uprgba;
  std::unique_ptr<eunomia::PictureIndexed> upindx;


  if (!loadImage(src, uppict, uprgba, upindx)) {
    std::cerr << "ファイル" << src << "の讀み込みに失敗した。" << std::endl;
    return 1;
  }


  if (upindx) { // PictureIndexedは擴大縮小ができないのでPictureにしてしまふ
    uppict = upindx->duplicatePicture();
  }

  if (uppict) {
    bool issmall;
    if (isP) {
      issmall = r < 1.0;
      w = uppict->width() * r;
      h = uppict->height() * r;
    }
    else {
      issmall = uppict->width() > w && uppict->height() > h;
    }
    
    auto pict2 = issmall ? uppict->reduce(w, h) : uppict->magnify(w, h);

    if (!saveImage(*pict2, dst)) {
      std::cerr << "ファイル" << dst << "への保存に失敗。" << std::endl;
      return 1;
    }
  }
  else if (uprgba) {
    bool issmall;
    if (isP) {
      issmall = r < 1.0;
      w = uprgba->width() * r;
      h = uprgba->height() * r;
    }
    else {
      issmall = uprgba->width() > w && uprgba->height() > h;
    }
    
    auto pict2 = issmall ? uprgba->reduce(w, h) : uprgba->magnify(w, h);

    if (!saveImage(*pict2, dst)) {
      std::cerr << "ファイル" << dst << "への保存に失敗。" << std::endl;
      return 1;
    }
  }
  else {
    std::cerr << "畫像の讀み込みに失敗した。" << std::endl;
    return 1;  
  }


  return 0;
}


//eof
