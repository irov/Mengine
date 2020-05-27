#pragma once

#ifndef MENGINE_MAX_PATH
#define MENGINE_MAX_PATH 260
#endif

#ifndef MENGINE_PATH_DELIM
#define MENGINE_PATH_DELIM '/'
#endif

#ifndef MENGINE_PATH_WDELIM
#define MENGINE_PATH_WDELIM L'/'
#endif

#ifndef MENGINE_MAX
#define MENGINE_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MENGINE_MIN
#define MENGINE_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MENGINE_CLAMP
#define MENGINE_CLAMP(a, b, c) MENGINE_MAX( MENGINE_MIN(b, c), a )
#endif

#define MENGINE_STRING_EMPTY ""

#define MENGINE_PP_STRINGIZE_I(X) #X
#define MENGINE_PP_STRINGIZE(X) MENGINE_PP_STRINGIZE_I(X)

#define MENGINE_PP_CONCATENATE_I(X, Y) X ## Y
#define MENGINE_PP_CONCATENATE(X, Y) MENGINE_PP_CONCATENATE_I(X, Y)

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)X)
#endif

#if defined(_MSC_VER)
#   define MENGINE_COMPILER_MSVC
#elif defined(__GNUC__)
#   define MENGINE_COMPILER_GCC
#elif defined(__clang__)
#   define MENGINE_COMPILER_CLANG
#elif defined(__EMSCRIPTEN__)
#   define MENGINE_COMPILER_EMSCRIPTEN
#elif defined(__MINGW32__)
#   define MENGINE_COMPILER_MINGW32
#elif defined(__MINGW64__)
#   define MENGINE_COMPILER_MINGW64
#else
#   error "undefine compiler"
#endif

#ifdef MENGINE_COMPILER_GCC
#define MENGINE_CODE_FILE __FILE__
#define MENGINE_CODE_FUNCTION __PRETTY_FUNCTION__
#define MENGINE_CODE_LINE __LINE__
#else
#define MENGINE_CODE_FILE __FILE__
#define MENGINE_CODE_FUNCTION __FUNCTION__
#define MENGINE_CODE_LINE __LINE__
#endif

#ifndef NDEBUG
#define MENGINE_DEBUG
#else
#define MENGINE_RELEASE
#endif

#ifndef MENGINE_DEBUG
#define MENGINE_RELEASE_UNUSED(X) MENGINE_UNUSED(X)
#else
#define MENGINE_RELEASE_UNUSED(X)
#endif

#ifdef MENGINE_DEBUG
#define MENGINE_DEBUG_VALUE(X, Y) (X)
#else
#define MENGINE_DEBUG_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_MASTER_RELEASE
#define MENGINE_MASTER_VALUE(X, Y) (X)
#else
#define MENGINE_MASTER_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_BUILD_PUBLISH
#define MENGINE_PUBLISH_VALUE(X, Y) (X)
#else
#define MENGINE_PUBLISH_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_DEBUG
#define MENGINE_DEBUG_ARGUMENTS(...) __VA_ARGS__
#else
#define MENGINE_DEBUG_ARGUMENTS(...)
#endif

#define MENGINE_CONSTEXPR constexpr
#define MENGINE_INLINE inline

#if defined(WIN32)
#   define MENGINE_PLATFORM_WINDOWS
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__linux__) && !defined(__ANDROID__)
#   define MENGINE_PLATFORM_LINUX
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_OS_OSX
#       define MENGINE_PLATFORM_OSX
#       define MENGINE_PLATFORM_DESKTOP
#   elif TARGET_OS_IPHONE
#       define MENGINE_PLATFORM_IOS
#       define MENGINE_PLATFORM_MOBILE
#   endif
#elif defined(__ANDROID__)
#   define MENGINE_PLATFORM_ANDROID
#   define MENGINE_PLATFORM_MOBILE
#else
#   error "undefine platform"
#endif

#if defined(__MINGW32__)
#   define MENGINE_TOOLCHAIN_MINGW
#elif defined(_MSC_VER)
#   define MENGINE_TOOLCHAIN_MSVC
#endif

#ifdef MENGINE_DEBUG
#   if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_TOOLCHAIN_MINGW)
#       define MENGINE_WINDOWS_DEBUG
#   endif
#endif

#ifndef MENGINE_ASSERTION_DEBUG
#   ifdef MENGINE_DEBUG
#       define MENGINE_ASSERTION_DEBUG
#   endif
#endif

#ifndef MENGINE_LOGGER_DEBUG
#   ifdef MENGINE_DEBUG
#       define MENGINE_LOGGER_DEBUG
#   endif
#endif

#define MENGINE_UNKNOWN_SIZE (~0U)
#define MENGINE_UNKNOWN_HASH (-1)

#ifndef MENGINE_PATH_INVALID_LENGTH
#define MENGINE_PATH_INVALID_LENGTH (~0U)
#endif

#ifndef MENGINE_MASTER_RELEASE
#define MENGINE_MASTER_DEVELOPMENT
#endif

#if defined(MENGINE_PLATFORM_OSX)
#   define off64_t off_t
#   define fopen64 fopen
#endif

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_TOOLCHAIN_MSVC)
#       if _MSC_VER >= 1400
#           include <sal.h>
#           if _MSC_VER > 1400
#               define MENGINE_CHECK_FORMAT_STRING(p) _Printf_format_string_ p
#           else
#               define MENGINE_CHECK_FORMAT_STRING(p) __format_string p
#           endif
#       else
#           define MENGINE_CHECK_FORMAT_STRING(p) p
#       endif
#   else
#       define MENGINE_CHECK_FORMAT_STRING(p) p
#   endif
#else
#   define MENGINE_CHECK_FORMAT_STRING(p) p
#endif

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_COMPILER_GCC)
#       define MENGINE_ATTRIBUTE_FORMAT_STRING(Format, Args) __attribute__ (( __format__ (printf, Format, Args) ))
#   else
#       define MENGINE_ATTRIBUTE_FORMAT_STRING(Format, Args)
#   endif
#else
#   define MENGINE_ATTRIBUTE_FORMAT_STRING(Format, Args)
#endif