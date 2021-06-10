cmake_minimum_required(VERSION 3.10)
project(eunomia CXX)
set(CMAKE_VERBOSE_MAKEFILE 1)

enable_language(CXX)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ZLIB)
find_package(PNG)
find_package(JPEG)


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
endif()


# リリースビルドライブラリ
add_library(eunomia STATIC ${EUNOMIA_SOURCES})
set_target_properties(eunomia
  PROPERTIES PUBLIC_HEADER "${EUNOMIA_HEADERS}"
)
target_compile_options(eunomia PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -O3>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -O3>
)
target_compile_definitions(eunomia PRIVATE NDEBUG)
target_include_directories(eunomia PRIVATE
  $<$<BOOL:PNG_FOUND>:${PNG_INCLUDE_DIRS}>
  $<$<BOOL:ZLIB_FOUND>:${ZLIB_INCLUDE_DIRS}>
  $<$<BOOL:JPEG_FOUND>:${JPEG_INCLUDE_DIRS}>
)
target_link_libraries(eunomia PRIVATE
  $<$<BOOL:PNG_FOUND>:png>
  $<$<BOOL:ZLIB_FOUND>:z>
  $<$<BOOL:JPEG_FOUND>:jpeg>
)


# デバッグビルドライブラリ
add_library(eunomia-debug ${EUNOMIA_SOURCES})
target_compile_options(eunomia-debug PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -g3 -ggdb>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -g3 -ggdb>
)
target_include_directories(eunomia-debug PRIVATE
  $<$<BOOL:PNG_FOUND>:${PNG_INCLUDE_DIRS}>
  $<$<BOOL:ZLIB_FOUND>:${ZLIB_INCLUDE_DIRS}>
  $<$<BOOL:JPEG_FOUND>:${JPEG_INCLUDE_DIRS}>
)
target_link_libraries(eunomia-debug PRIVATE
  $<$<BOOL:PNG_FOUND>:png>
  $<$<BOOL:ZLIB_FOUND>:z>
  $<$<BOOL:JPEG_FOUND>:jpeg>
)


# インストール設定
install(TARGETS eunomia eunomia-debug
  EXPORT eunomia-export
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
  PUBLIC_HEADER DESTINATION include/eunomia
)

install(EXPORT eunomia-export
  FILE eunomia-config.cmake
  DESTINATION share/cmake/eunomia/
#  EXPORT_LINK_INTERFACE_LIBRARIES
)


#### resizer ########
if (ZLIB_FOUND AND PNG_FOUND AND JPEG_FOUND)
  add_executable(resizer
    resizer.cpp
  )
  target_compile_options(resizer PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall -O3>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -O3>
  )
  target_include_directories(resizer PRIVATE
    ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS}
  )
  target_link_options(resizer PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-static>
    $<$<CXX_COMPILER_ID:GNU>:-static>
  )
  target_compile_definitions(resizer PRIVATE NDEBUG)
  target_link_libraries(resizer eunomia png z)

  #install(TARGETS resizer
  #  DESTINATION bin
  #)
endif()

if (ZLIB_FOUND AND PNG_FOUND)
  add_executable(hextest
    hextest.cpp
  )
  target_compile_options(hextest PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -O3>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -O3>
  )
  target_compile_definitions(hextest PRIVATE NDEBUG)
  target_include_directories(hextest PRIVATE
    ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
  )
  target_link_options(hextest PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-static>
    $<$<CXX_COMPILER_ID:Clang>:-static>
  )
  target_link_libraries(hextest eunomia png z)

  #install(TARGETS hextest
  #  DESTINATION bin
  #)
endif()


#eof
