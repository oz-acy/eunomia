# CMakeLists.txt
#
# by oZ/acy
# Copyright 2021-2022 oZ/acy (名賀月晃嗣) <acy@hiemalis.org>
#
# Redistribution and use in source and binary forms, 
#     with or without modification, 
#   are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


cmake_minimum_required(VERSION 3.19)
project(eunomia CXX)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)


option(BUILD_SHARED_LIBS
  "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)"
  OFF
)

# staticなライブラリをビルドする場合は、依存ライブラリもstaticにする。
if (NOT BUILD_SHARED_LIBS AND NOT MSVC)
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a;.dll.a;.lib)
endif()


# 依存するライブラリ
#find_package(ZLIB)
find_package(PNG)
find_package(JPEG)


# デバッグバージョンのpostfix
set(CMAKE_DEBUG_POSTFIX "d")


# ソースファイルとヘッダファイル
set(EUNOMIA_SOURCES
  ibuf_blt.cpp
  picture.cpp
    pict_magnify.cpp
    pict_reduce.cpp
    pict_grayscaled.cpp
    pict_dupl_pictidx.cpp
  picture_indexed.cpp
    pictidx_dupl_pict.cpp
  picture_rgba.cpp
    pictrgba_magnify.cpp
    pictrgba_reduce.cpp
    pictrgba_grayscaled.cpp
    pictrgba_stripalpha.cpp
    pictrgba_dupl_pictidx.cpp
  dibout.cpp
  dibin.cpp
)
set(EUNOMIA_HEADERS
  exception.h
  debuglogger.h
  noncopyable.h
  scopeguard.h
  utility.h
  rect.h
  imagebuffer.h
    ibuf_blt.h
    ibuf_draw.h
  colour.h
  picture.h
  picture_indexed.h
  picture_rgba.h
  hexpainter.h
  dibio.h
)
set(EUNOMIA_PRIVATE_HEADERS
  pict_indexing.h
  pict_magnify_func.h
)

if (PNG_FOUND)
  list(APPEND EUNOMIA_SOURCES
    pngin.cpp
    pngout.cpp
  )
  list(APPEND EUNOMIA_HEADERS
    pngio.h
  )
endif()

if (JPEG_FOUND OR libjpeg-turbo_FOUND)
  list(APPEND EUNOMIA_SOURCES
    jpegio.cpp
    jpegin.cpp
    jpegout.cpp
  )
  list(APPEND EUNOMIA_HEADERS
    jpegio.h
  )
  list(APPEND EUNOMIA_PRIVATE_HEADERS
    jpegio_implement.h
  )
endif()


#### libeunomia ########
add_library(eunomia ${EUNOMIA_SOURCES})
set_target_properties(eunomia
  PROPERTIES PUBLIC_HEADER "${EUNOMIA_HEADERS}"
)
if(MSVC)
  set_target_properties(eunomia PROPERTIES PREFIX "lib")
  target_compile_options(eunomia PRIVATE /source-charset:utf-8)
endif()
target_link_libraries(eunomia PRIVATE PNG::PNG JPEG::JPEG)


# インストール設定
install(TARGETS eunomia
  EXPORT eunomia-export
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/eunomia
)

install(EXPORT eunomia-export
  FILE eunomia-config.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/eunomia/
  NAMESPACE eunomia::
  EXPORT_LINK_INTERFACE_LIBRARIES
)




#### resizer ########
if (PNG_FOUND AND JPEG_FOUND)
  add_executable(resizer
    resizer.cpp
  )
  target_link_libraries(resizer eunomia)
  target_compile_options(resizer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>
  )
  target_link_options(resizer PRIVATE
    $<$<AND:$<BOOL:${MINGW}>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-static>
    #$<$<BOOL:${MINGW}>:-static>
    #$<$<CXX_COMPILER_ID:MSVC>:/NODEFAULTLIB:libcmt>
  )
  #if(MSVC)
  #  target_compile_options(resizer PRIVATE /source-charset:utf-8)
  #endif()
  #install(TARGETS resizer
  #  DESTINATION ${CMAKE_INSTALL_BINDIR}
  #)
endif()


#### hextest ########
if (PNG_FOUND)
  add_executable(hextest
    hextest.cpp
  )
  target_link_libraries(hextest eunomia)
  target_compile_options(hextest PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>
  )
  target_link_options(hextest PRIVATE
    $<$<AND:$<BOOL:${MINGW}>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-static>
    #$<$<BOOL:${MINGW}>:-static>
    #$<$<CXX_COMPILER_ID:MSVC>:/NODEFAULTLIB:libcmt>
  )
  #if(MSVC)
  #  target_compile_options(hextest PRIVATE /source-charset:utf-8)
  #endif()
  #install(TARGETS hextest
  #  DESTINATION ${CMAKE_INSTALL_BINDIR}
  #)
endif()




#eof
