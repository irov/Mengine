#  Copyright (c) 2020 Google LLC.
#
#  Use of this source code is governed by a BSD-style license
#  that can be found in the LICENSE file in the root of the source
#  tree. An additional intellectual property rights grant can be found
#  in the file PATENTS.  All contributing project authors may
#  be found in the AUTHORS file in the root of the source tree.

cmake_minimum_required(VERSION 3.7)

if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

project(WebP C)

# Options for coder / decoder executables.
if(BUILD_SHARED_LIBS)
  set(WEBP_LINK_STATIC_DEFAULT OFF)
else()
  set(WEBP_LINK_STATIC_DEFAULT ON)
endif()
option(WEBP_LINK_STATIC
       "Link using static libraries. If OFF, use dynamic libraries."
       ${WEBP_LINK_STATIC_DEFAULT})
if(NOT EMSCRIPTEN)
  # Disable SIMD on Emscripten by default, as it's a new unstable Wasm feature.
  # Users can still explicitly opt-in to make a SIMD-enabled build.
  set(WEBP_ENABLE_SIMD_DEFAULT ON)
endif()
option(WEBP_ENABLE_SIMD "Enable any SIMD optimization."
       ${WEBP_ENABLE_SIMD_DEFAULT})
option(WEBP_BUILD_ANIM_UTILS "Build animation utilities." ON)
option(WEBP_BUILD_CWEBP "Build the cwebp command line tool." ON)
option(WEBP_BUILD_DWEBP "Build the dwebp command line tool." ON)
option(WEBP_BUILD_GIF2WEBP "Build the gif2webp conversion tool." ON)
option(WEBP_BUILD_IMG2WEBP "Build the img2webp animation tool." ON)
option(WEBP_BUILD_VWEBP "Build the vwebp viewer tool." ON)
option(WEBP_BUILD_WEBPINFO "Build the webpinfo command line tool." ON)
option(WEBP_BUILD_LIBWEBPMUX "Build the libwebpmux library." ON)
option(WEBP_BUILD_WEBPMUX "Build the webpmux command line tool." ON)
option(WEBP_BUILD_EXTRAS "Build extras." ON)
option(WEBP_BUILD_WEBP_JS "Emscripten build of webp.js." OFF)
option(WEBP_USE_THREAD "Enable threading support" ON)
option(WEBP_NEAR_LOSSLESS "Enable near-lossless encoding" ON)
option(WEBP_ENABLE_SWAP_16BIT_CSP "Enable byte swap for 16 bit colorspaces."
       OFF)
set(WEBP_BITTRACE "0" CACHE STRING "Bit trace mode (0=none, 1=bit, 2=bytes)")
set_property(CACHE WEBP_BITTRACE PROPERTY STRINGS 0 1 2)

if(WEBP_LINK_STATIC)
  if(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  else()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  endif()
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  # vwebp does not compile on Ubuntu with static libraries so disabling it for
  # now.
  set(WEBP_BUILD_VWEBP OFF)
endif()

# Option needed for handling Unicode file names on Windows.
if(WIN32)
  option(WEBP_UNICODE "Build Unicode executables." ON)
endif()

if(WEBP_BUILD_WEBP_JS)
  set(WEBP_BUILD_ANIM_UTILS OFF)
  set(WEBP_BUILD_CWEBP OFF)
  set(WEBP_BUILD_DWEBP OFF)
  set(WEBP_BUILD_GIF2WEBP OFF)
  set(WEBP_BUILD_IMG2WEBP OFF)
  set(WEBP_BUILD_VWEBP OFF)
  set(WEBP_BUILD_WEBPINFO OFF)
  set(WEBP_BUILD_WEBPMUX OFF)
  set(WEBP_BUILD_EXTRAS OFF)
  set(WEBP_USE_THREAD OFF)

  if(WEBP_ENABLE_SIMD)
    message("wasm2js does not support SIMD, disabling webp.js generation.")
  endif()
endif()

set(WEBP_DEP_LIBRARIES)
set(WEBP_DEP_INCLUDE_DIRS)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
      CACHE STRING "Build type: Release, Debug, MinSizeRel or RelWithDebInfo"
      FORCE)
endif()

# Include dependencies.
include(cmake/deps.cmake)
include(GNUInstallDirs)

# ##############################################################################
# Options.
if(WEBP_ENABLE_SWAP_16BIT_CSP)
  add_definitions(-DWEBP_SWAP_16BIT_CSP=1)
endif()

if(NOT WEBP_BITTRACE STREQUAL "0")
  add_definitions(-DBITTRACE=${WEBP_BITTRACE})
endif()

if(WEBP_UNICODE)
  # Windows recommends setting both UNICODE and _UNICODE.
  add_definitions(-DUNICODE -D_UNICODE)
endif()

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{prefix\}/lib")
set(includedir "\$\{prefix\}/include")
set(PTHREAD_LIBS ${CMAKE_THREAD_LIBS_INIT})
set(INSTALLED_LIBRARIES)

set(CMAKE_C_VISIBILITY_PRESET hidden)

# ##############################################################################
# Android only.
if(ANDROID)
  include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
  add_library(cpufeatures STATIC
              ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
  list(APPEND INSTALLED_LIBRARIES cpufeatures)
  target_link_libraries(cpufeatures dl)
  set(WEBP_DEP_LIBRARIES ${WEBP_DEP_LIBRARIES} cpufeatures)
  set(WEBP_DEP_INCLUDE_DIRS ${WEBP_DEP_INCLUDE_DIRS}
      ${ANDROID_NDK}/sources/android/cpufeatures)
  add_definitions(-DHAVE_CPU_FEATURES_H=1)
  set(HAVE_CPU_FEATURES_H 1)
else()
  set(HAVE_CPU_FEATURES_H 0)
endif()

function(configure_pkg_config FILE)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in"
                 "${THIRDPARTY_CONFIG_DIR}/libwebp/${FILE}")

  if(HAVE_MATH_LIBRARY)
    # MSVC doesn't have libm
    file(READ ${THIRDPARTY_CONFIG_DIR}/libwebp/${FILE} data)
    string(REPLACE "-lm" "" data ${data})
    file(WRITE ${THIRDPARTY_CONFIG_DIR}/libwebp/${FILE} ${data})
  endif()

  install(
    FILES "${THIRDPARTY_CONFIG_DIR}/libwebp/${FILE}"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
endfunction()

# ##############################################################################
# WebP source files. Read the Makefile.am to get the source files.

# We expect the Makefiles to define the sources as defined in the first regex.
# E.g.: libimagedec_la_SOURCES  = image_dec.c image_dec.h
function(parse_Makefile_am FOLDER VAR SRC_REGEX)
  file(READ ${FOLDER}/Makefile.am MAKEFILE_AM)
  string(REGEX MATCHALL
               "${SRC_REGEX}_SOURCES[ ]*\\+?=[ ]+[0-9a-z\\._ ]*"
               FILES_PER_LINE
               ${MAKEFILE_AM})
  set(SRCS ${${VAR}})
  foreach(FILES ${FILES_PER_LINE})
    string(FIND ${FILES} "=" OFFSET)
    math(EXPR OFFSET "${OFFSET} + 2")
    string(SUBSTRING ${FILES}
                     ${OFFSET}
                     -1
                     FILES)
    if(FILES)
      string(REGEX MATCHALL
                   "[0-9a-z\\._]+"
                   FILES
                   ${FILES})
      foreach(FILE ${FILES})
        list(APPEND SRCS ${FOLDER}/${FILE})
      endforeach()
    endif()
  endforeach()
  set(${VAR} ${SRCS} PARENT_SCOPE)
endfunction()

set(WEBP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
parse_makefile_am(${WEBP_SRC_DIR}/dec "WEBP_DEC_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/demux "WEBP_DEMUX_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_COMMON_SRCS" "COMMON")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_ENC_SRCS" "ENC")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_ENC_SRCS" "dsp_[^ ]*")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_DEC_SRCS" "decode_[^ ]*")
parse_makefile_am(${WEBP_SRC_DIR}/enc "WEBP_ENC_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_COMMON_SRCS" "COMMON")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_ENC_SRCS" "ENC")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_DEC_SRCS" "decode_[^ ]*")

# Remove the files specific to SIMD we don't use.
foreach(FILE ${WEBP_SIMD_FILES_NOT_TO_INCLUDE})
  list(REMOVE_ITEM WEBP_DSP_ENC_SRCS ${FILE})
  list(REMOVE_ITEM WEBP_DSP_DEC_SRCS ${FILE})
endforeach()

# Generate the config.h file.
if(WIN32 OR ANDROID)
    set(WEBP_HAVE_SSE41 0)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in
               ${THIRDPARTY_CONFIG_DIR}/libwebp/src/webp/config.h)

set(WEBP_DEP_INCLUDE_DIRS ${WEBP_DEP_INCLUDE_DIRS}
    ${THIRDPARTY_CONFIG_DIR}/libwebp)

add_definitions(-DHAVE_CONFIG_H)

# ##############################################################################
# Build the webpdecoder library.

# Creates a source file with an unused stub function in $CMAKE_BINARY_DIR and
# adds it to the specified target. Currently used only with Xcode.
#
# See also:
# https://cmake.org/cmake/help/v3.18/command/add_library.html#object-libraries
# "Some native build systems (such as Xcode) may not like targets that have
# only object files, so consider adding at least one real source file to any
# target that references $<TARGET_OBJECTS:objlib>."
function(libwebp_add_stub_file TARGET)
  set(stub_source_dir "${CMAKE_BINARY_DIR}")
  set(stub_source_file
      "${stub_source_dir}/libwebp_${TARGET}_stub.c")
  set(stub_source_code
      "// Generated file. DO NOT EDIT!\n"
      "// C source file created for target ${TARGET}.\n"
      "void libwebp_${TARGET}_stub_function(void)\;\n"
      "void libwebp_${TARGET}_stub_function(void) {}\n")
  file(WRITE "${stub_source_file}" ${stub_source_code})

  target_sources(${TARGET} PRIVATE ${stub_source_file})
endfunction()

parse_makefile_am(${CMAKE_CURRENT_SOURCE_DIR}/sharpyuv "WEBP_SHARPYUV_SRCS"
                  "")
add_library(sharpyuv OBJECT ${WEBP_SHARPYUV_SRCS})
target_include_directories(sharpyuv
                            PRIVATE ${THIRDPARTY_CONFIG_DIR}/libwebp
                            ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(
  sharpyuv
  PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/sharpyuv/sharpyuv.h;\
${CMAKE_CURRENT_SOURCE_DIR}/sharpyuv/sharpyuv_csp.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/types.h")

if(MSVC)
  # avoid security warnings for e.g., fopen() used in the examples.
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  add_definitions(-Wall)
endif()
include_directories(${WEBP_DEP_INCLUDE_DIRS})

add_library(webp
            ${WEBP_SHARPYUV_SRCS}
            ${WEBP_DEC_SRCS}
            ${WEBP_DSP_COMMON_SRCS}
            ${WEBP_DSP_DEC_SRCS}
            ${WEBP_DSP_ENC_SRCS}
            ${WEBP_ENC_SRCS}
            ${WEBP_UTILS_COMMON_SRCS}
            ${WEBP_UTILS_DEC_SRCS}
            ${WEBP_UTILS_ENC_SRCS})
if(XCODE)
  libwebp_add_stub_file(webp)
endif()
target_link_libraries(webp ${WEBP_DEP_LIBRARIES})
target_include_directories(
  webp
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
          ${THIRDPARTY_CONFIG_DIR}/libwebp
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
         $<INSTALL_INTERFACE:include>)
set_target_properties(
  webp
  PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/webp/decode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/encode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/types.h")

# Set the version numbers.
macro(set_version FILE TARGET_NAME NAME_IN_MAKEFILE)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/${FILE} SOURCE_FILE)
  string(REGEX MATCH
               "${NAME_IN_MAKEFILE}_la_LDFLAGS[^\n]* -version-info [0-9:]+"
               TMP
               ${SOURCE_FILE})
  string(REGEX MATCH
               "[0-9:]+"
               TMP
               ${TMP})
  string(REGEX
         REPLACE ":"
                 " "
                 LT_VERSION
                 ${TMP})

  # See the libtool docs for more information:
  # https://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
  #
  # c=<current>, a=<age>, r=<revision>
  #
  # libtool generates a .so file as .so.[c-a].a.r, while -version-info c:r:a is
  # passed to libtool.
  #
  # We set FULL = [c-a].a.r and MAJOR = [c-a].
  separate_arguments(LT_VERSION)
  list(GET LT_VERSION 0 LT_CURRENT)
  list(GET LT_VERSION 1 LT_REVISION)
  list(GET LT_VERSION 2 LT_AGE)
  math(EXPR LT_CURRENT_MINUS_AGE "${LT_CURRENT} - ${LT_AGE}")

  set_target_properties(
    ${TARGET_NAME}
    PROPERTIES VERSION
               ${LT_CURRENT_MINUS_AGE}.${LT_AGE}.${LT_REVISION}
               SOVERSION
               ${LT_CURRENT_MINUS_AGE})
endmacro()
set_version(Makefile.am webp webp)