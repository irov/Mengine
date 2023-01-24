#pragma once

#if defined(_MSC_VER)
#   define MENGINE_COMPILER_MSVC
#   if _MSC_VER >= 1930
#       define MENGINE_COMPILER_MSVC17
#   elif _MSC_VER >= 1920
#       define MENGINE_COMPILER_MSVC16
#   elif _MSC_VER >= 1910
#       define MENGINE_COMPILER_MSVC15
#   elif _MSC_VER >= 1900
#       define MENGINE_COMPILER_MSVC14
#   endif
#elif defined(__GNUC__)
#   define MENGINE_COMPILER_GCC
#elif defined(__clang__)
#   define MENGINE_COMPILER_CLANG
#elif defined(__EMSCRIPTEN__)
#   define MENGINE_COMPILER_EMSCRIPTEN
#elif defined(__MINGW32__)
#   define MENGINE_COMPILER_MINGW
#   ifdef defined(__MINGW64__)
#       define MENGINE_COMPILER_MINGW64
#   else
#       define MENGINE_COMPILER_MINGW32
#   endif
#else
#   error "undefine compiler"
#endif

#if defined(_WIN32)
#   define MENGINE_PLATFORM_WINDOWS
#   define MENGINE_PLATFORM_DESKTOP
#   if defined(_WIN64)
#       define MENGINE_PLATFORM_WINDOWS64
#   else
#       define MENGINE_PLATFORM_WINDOWS32
#   endif
#elif defined(__linux__) && !defined(__ANDROID__)
#   define MENGINE_PLATFORM_LINUX
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__APPLE__)
#   define MENGINE_PLATFORM_APPLE
#   include "TargetConditionals.h"
#   if TARGET_OS_OSX
#       define MENGINE_PLATFORM_MACOS
#       define MENGINE_PLATFORM_DESKTOP
#   elif TARGET_OS_IPHONE
#       define MENGINE_PLATFORM_IOS
#       define MENGINE_PLATFORM_MOBILE
#       if TARGET_OS_SIMULATOR
#           define MENGINE_PLATFORM_IOS_SIMULATOR
#       endif
#   endif
#elif defined(__ANDROID__)
#   define MENGINE_PLATFORM_ANDROID
#   define MENGINE_PLATFORM_MOBILE
#else
#   error "undefine platform"
#endif

#if defined(__MINGW64__)
#   define MENGINE_TOOLCHAIN_MINGW
#   define MENGINE_TOOLCHAIN_MINGW64
#elif defined(__MINGW32__)
#   define MENGINE_TOOLCHAIN_MINGW
#   define MENGINE_TOOLCHAIN_MINGW32
#elif defined(_MSC_VER)
#   define MENGINE_TOOLCHAIN_MSVC
#endif

#ifndef NDEBUG
#   define MENGINE_DEBUG
#else
#   define MENGINE_RELEASE
#endif

#ifdef MENGINE_DEBUG
#   if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_TOOLCHAIN_MINGW)
#       define MENGINE_WINDOWS_DEBUG
#   endif
#endif

#ifndef MENGINE_CODE_FILE
#define MENGINE_CODE_FILE __FILE__
#endif

#ifndef MENGINE_CODE_LINE
#define MENGINE_CODE_LINE __LINE__
#endif

#ifndef MENGINE_CODE_FUNCTION
#   ifdef MENGINE_COMPILER_GCC
#       define MENGINE_CODE_FUNCTION __PRETTY_FUNCTION__
#   else
#       define MENGINE_CODE_FUNCTION __FUNCTION__
#   endif
#endif

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)sizeof(X))
#endif

#ifndef MENGINE_NOP
#define MENGINE_NOP [](){}()
#endif

#ifndef MENGINE_DEBUG
#   define MENGINE_RELEASE_UNUSED(X) MENGINE_UNUSED(X)
#else
#   define MENGINE_RELEASE_UNUSED(X)
#endif

#ifdef MENGINE_WINDOWS_DEBUG
#   define MENGINE_WINDOWS_DEBUG_VALUE(X, Y) (X)
#else
#   define MENGINE_WINDOWS_DEBUG_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_DEBUG
#   define MENGINE_DEBUG_VALUE(X, Y) (X)
#else
#   define MENGINE_DEBUG_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_MASTER_RELEASE
#   define MENGINE_MASTER_RELEASE_VALUE(X, Y) (X)
#else
#   define MENGINE_MASTER_RELEASE_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_BUILD_PUBLISH
#   define MENGINE_BUILD_PUBLISH_VALUE(X, Y) (X)
#else
#   define MENGINE_BUILD_PUBLISH_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_DEBUG
#   define MENGINE_DEBUG_ARGUMENTS(...) __VA_ARGS__
#else
#   define MENGINE_DEBUG_ARGUMENTS(...)
#endif

#ifndef MENGINE_RENDER_CHECK_ERROR
#   ifdef MENGINE_DEBUG
#       define MENGINE_RENDER_CHECK_ERROR 1
#   else
#       define MENGINE_RENDER_CHECK_ERROR 0
#   endif
#endif

#if MENGINE_RENDER_CHECK_ERROR == 1
#   define MENGINE_RENDER_CHECK_ERROR_ENABLE
#endif

#ifndef MENGINE_ASSERTION_DEBUG
#   ifdef MENGINE_DEBUG
#       define MENGINE_ASSERTION_DEBUG 1
#   else
#       define MENGINE_ASSERTION_DEBUG 0
#   endif
#endif

#if MENGINE_ASSERTION_DEBUG == 1
#   define MENGINE_ASSERTION_DEBUG_ENABLE
#endif

#ifndef MENGINE_DOCUMENT
#   ifdef MENGINE_DEBUG
#       define MENGINE_DOCUMENT 1
#   else
#       define MENGINE_DOCUMENT 0
#   endif
#endif

#if MENGINE_DOCUMENT == 1
#   define MENGINE_DOCUMENT_ENABLE
#endif

#ifdef MENGINE_DOCUMENT_ENABLE
#   define MENGINE_DOCUMENT_VALUE(X, Y) (X)
#else
#   define MENGINE_DOCUMENT_VALUE(X, Y) (Y)
#endif

#ifdef MENGINE_DOCUMENT_ENABLE
#   define MENGINE_DOCUMENT_ARGUMENTS(...) __VA_ARGS__
#else
#   define MENGINE_DOCUMENT_ARGUMENTS(...)
#endif

#ifndef MENGINE_CONSTEXPR
#define MENGINE_CONSTEXPR constexpr
#endif

#ifndef MENGINE_INLINE
#define MENGINE_INLINE inline
#endif

#ifndef MENGINE_THREAD_LOCAL
#   ifndef MENGINE_PLATFORM_IOS
#       define MENGINE_THREAD_LOCAL thread_local
#   else
#       define MENGINE_THREAD_LOCAL
#   endif
#endif

#ifndef MENGINE_PROFILER_ENABLE
#   ifdef MENGINE_WINDOWS_DEBUG
#       define MENGINE_PROFILER_ENABLE
#   endif
#endif

#ifndef MENGINE_UNKNOWN_SIZE
#define MENGINE_UNKNOWN_SIZE (~0U)
#endif

#ifndef MENGINE_UNKNOWN_HASH
#define MENGINE_UNKNOWN_HASH (-1)
#endif

#ifndef MENGINE_PATH_INVALID_LENGTH
#define MENGINE_PATH_INVALID_LENGTH (~0U)
#endif

#ifndef MENGINE_MASTER_RELEASE
#define MENGINE_MASTER_DEVELOPMENT
#endif

#if defined(MENGINE_PLATFORM_MACOS)
#   define off64_t off_t
#   define fopen64 fopen
#endif

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_TOOLCHAIN_MSVC)
#       if _MSC_VER >= 1400
#           include <sal.h>
#           if _MSC_VER > 1400
#               define MENGINE_CHECK_FORMAT_STRING(Format) _Printf_format_string_ Format
#           else
#               define MENGINE_CHECK_FORMAT_STRING(Format) __format_string Format
#           endif
#       else
#           define MENGINE_CHECK_FORMAT_STRING(Format) Format
#       endif
#   else
#       define MENGINE_CHECK_FORMAT_STRING(Format) Format
#   endif
#else
#   define MENGINE_CHECK_FORMAT_STRING(Format) Format
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

#ifndef MENGINE_MAX_PATH
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_MAX_PATH 260
#   elif defined(MENGINE_PLATFORM_LINUX)
#       define MENGINE_MAX_PATH 1024
#   elif defined(MENGINE_PLATFORM_MACOS)
#       define MENGINE_MAX_PATH 1024
#   elif defined(MENGINE_PLATFORM_IOS)
#       define MENGINE_MAX_PATH 1024
#   elif defined(MENGINE_PLATFORM_ANDROID)
#       define MENGINE_MAX_PATH 1024
#   else
#       error "undefine MENGINE_MAX_PATH for this platform"
#   endif
#endif

#ifndef MENGINE_PATH_DELIM
#define MENGINE_PATH_DELIM '/'
#endif

#ifndef MENGINE_PATH_WDELIM
#define MENGINE_PATH_WDELIM L'/'
#endif

#ifndef MENGINE_OFFSETOF
#define MENGINE_OFFSETOF(T, V) offsetof(T, V)
#endif

#ifndef MENGINE_PVOID_OFFSET
#define MENGINE_PVOID_OFFSET(P, O) (reinterpret_cast<unsigned char *>(P) + (O))
#endif

#ifndef MENGINE_CPVOID_OFFSET
#define MENGINE_CPVOID_OFFSET(P, O) (reinterpret_cast<const unsigned char *>(P) + (O))
#endif

#ifndef MENGINE_FOURCC
#define MENGINE_FOURCC(c0, c1, c2, c3) (((c0) << 0) | ((c1) << 8) | ((c2) << 16) | ((c3) << 24))
#endif

#ifndef MENGINE_MAX
#define MENGINE_MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MENGINE_MIN
#define MENGINE_MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MENGINE_MAX_ALIGNED_POINTER_VALUE
#   ifdef MENGINE_ENVIRONMENT_ARCHITECTURE_X86
#       define MENGINE_MAX_ALIGNED_POINTER_VALUE (0xfffffffc)
#   elif MENGINE_ENVIRONMENT_ARCHITECTURE_X64
#       define MENGINE_MAX_ALIGNED_POINTER_VALUE (0xfffffffffffffff8)
#   else
#       error "invalid define MENGINE_MAX_ALIGNED_POINTER_VALUE [unsupport architecture]"
#   endif
#endif

#ifndef MENGINE_POWER2
#define MENGINE_POWER2(N) (1 << N)
#endif

#ifndef MENGINE_STATIC_STRING_LENGTH
#define MENGINE_STATIC_STRING_LENGTH(S) (sizeof( (S) ) - 1)
#endif

#ifndef MENGINE_CLAMP
#define MENGINE_CLAMP(A, B, C) MENGINE_MAX( A, MENGINE_MIN(B, C) )
#endif

#ifndef MENGINE_STRING_EMPTY
#define MENGINE_STRING_EMPTY ""
#endif

#ifndef MENGINE_PP_STRINGIZE
#define MENGINE_PP_STRINGIZE_I(X) #X
#define MENGINE_PP_STRINGIZE(X) MENGINE_PP_STRINGIZE_I(X)
#endif

#ifndef MENGINE_PP_CONCATENATE
#define MENGINE_PP_CONCATENATE_I(X, Y) X ## Y
#define MENGINE_PP_CONCATENATE(X, Y) MENGINE_PP_CONCATENATE_I(X, Y)
#endif