cmake_minimum_required(VERSION 3.19)
project(eunomia CXX)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

find_package(ZLIB)
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
  noncopyable.h
  utility.h
  debuglogger.h
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

if (ZLIB_FOUND AND PNG_FOUND)
  list(APPEND EUNOMIA_SOURCES
    pngin.cpp
    pngout.cpp
  )
  list(APPEND EUNOMIA_HEADERS
    pngio.h
  )
endif()

if (JPEG_FOUND)
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


# build targets
add_library(eunomia STATIC ${EUNOMIA_SOURCES})
set_target_properties(eunomia
  PROPERTIES PUBLIC_HEADER "${EUNOMIA_HEADERS}"
)
if(MSVC)
  set_target_properties(eunomia PROPERTIES PREFIX "lib")
  target_compile_options(eunomia PRIVATE /source-charset:utf-8)
endif()
# 靜的ライブラリの指定を姑息に解決
if(MSVC)
  target_link_libraries(eunomia PRIVATE
    PNG::PNG
    JPEG::JPEG
  )
else()
  target_link_libraries(eunomia PRIVATE
    png z jpeg
  )
endif()


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
  #EXPORT_LINK_INTERFACE_LIBRARIES
)




#### resizer ########
if (ZLIB_FOUND AND PNG_FOUND AND JPEG_FOUND)
  add_executable(resizer
    resizer.cpp
  )
  target_link_options(resizer PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-static>
    $<$<CXX_COMPILER_ID:GNU>:-static>
  )
  target_link_libraries(resizer eunomia)
  if(MSVC)
    target_compile_options(resizer PRIVATE /source-charset:utf-8)
  endif()
  #install(TARGETS resizer
  #  DESTINATION bin
  #)
endif()


#### hextest ########
if (ZLIB_FOUND AND PNG_FOUND)
  add_executable(hextest
    hextest.cpp
  )
  target_link_options(hextest PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-static>
    $<$<CXX_COMPILER_ID:Clang>:-static>
  )
  target_link_libraries(hextest eunomia)
  if(MSVC)
    target_compile_options(hextest PRIVATE /source-charset:utf-8)
  endif()

  #install(TARGETS hextest
  #  DESTINATION bin
  #)
endif()




#eof
