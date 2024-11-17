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
#   if defined(__MINGW64__)
#       define MENGINE_COMPILER_MINGW64
#   else
#       define MENGINE_COMPILER_MINGW32
#   endif
#else
#   error "undefine compiler"
#endif

#if defined(_M_IX86) || defined(__i386__)
#   define MENGINE_MACHINE_IX86 _M_IX86
#endif

#if defined(_M_X64) || defined(__x86_64__)
#   define MENGINE_MACHINE_X64 _M_X64
#endif

#if defined(_M_IA64) || defined(__ia64__)
#   define MENGINE_MACHINE_IA64 _M_IA64
#endif

#if defined(_M_AMD64) || defined(__amd64__)
#   define MENGINE_MACHINE_AMD64 _M_AMD64
#endif

#if defined(_M_ARM) || defined(__arm__)
#   define MENGINE_MACHINE_ARM _M_ARM64
#endif

#if defined(_M_ARM64) || defined(__aarch64__)
#   define MENGINE_MACHINE_ARM64 _M_ARM64
#endif

#if defined(_WIN32)
#   define MENGINE_PLATFORM_WINDOWS
#   define MENGINE_PLATFORM_DESKTOP
#   if defined(_WIN64)
#       define MENGINE_PLATFORM_WINDOWS64
#   else
#       define MENGINE_PLATFORM_WINDOWS32
#   endif
#   include <winapifamily.h>
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PC_APP)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_PC_APP
#   endif
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_PHONE_APP
#   endif
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_APP
#   endif
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_DESKTOP
#   endif
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_SYSTEM)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_SYSTEM
#   endif
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_GAMES)
#       define MENGINE_PLATFORM_WINDOWS_FAMILY_GAMES
#   endif
#   if !defined(MENGINE_PLATFORM_WINDOWS_FAMILY_DESKTOP)
#       define MENGINE_PLATFORM_UWP
#   endif
#elif defined(__linux__) && !defined(__ANDROID__)
#   define MENGINE_PLATFORM_LINUX
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__APPLE__)
#   define MENGINE_PLATFORM_APPLE
#   include <TargetConditionals.h>
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

#if defined(__OBJC__)
#   define MENGINE_COMPILE_OBJECTIVE_C
#endif

#ifndef NDEBUG
#   define MENGINE_DEBUG
#else
#   define MENGINE_RELEASE
#endif

#ifndef MENGINE_SETJMP_UNSUPPORTED
#   define MENGINE_SETJMP_SUPPORTED
#endif

#ifndef MENGINE_RTTI
#   if defined(MENGINE_COMPILER_GCC) || defined(MENGINE_COMPILER_CLANG)
#       ifdef __GXX_RTTI
#           define MENGINE_RTTI 1
#       else
#           define MENGINE_RTTI 0
#       endif
#   elif defined(MENGINE_COMPILER_MSVC)
#       ifdef _CPPRTTI
#           define MENGINE_RTTI 1
#       else
#           define MENGINE_RTTI 0
#       endif
#   else
#       error "undefine MENGINE_RTTI for this compiler"
#   endif
#endif


#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_TOOLCHAIN_MINGW)
#       define MENGINE_WINDOWS_DEBUG
#   endif
#endif

#if defined(MENGINE_RELEASE)
#   if defined(MENGINE_COMPILE_OBJECTIVE_C)
#       define NS_BLOCK_ASSERTIONS
#   endif
#endif

#ifndef MENGINE_CODE_FILE
#define MENGINE_CODE_FILE __FILE__
#endif

#ifndef MENGINE_CODE_LINE
#define MENGINE_CODE_LINE __LINE__
#endif

#ifndef MENGINE_CODE_LIBRARY
#define MENGINE_CODE_LIBRARY MENGINE_LIBRARY_NAME
#endif

#ifndef MENGINE_CODE_FUNCTION
#   if defined(MENGINE_COMPILER_GCC) || defined(MENGINE_COMPILER_CLANG)
#       define MENGINE_CODE_FUNCTION __PRETTY_FUNCTION__
#   elif defined(MENGINE_COMPILER_MSVC)
#       define MENGINE_CODE_FUNCTION __FUNCTION__
#   else
#       define MENGINE_CODE_FUNCTION __FUNCTION__
#   endif
#endif

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)(X))
#endif

#ifndef MENGINE_NOP
#define MENGINE_NOP [](){}()
#endif

#if defined(MENGINE_RELEASE)
#   define MENGINE_RELEASE_UNUSED(X) MENGINE_UNUSED(X)
#else
#   define MENGINE_RELEASE_UNUSED(X)
#endif

#if defined(MENGINE_WINDOWS_DEBUG)
#   define MENGINE_WINDOWS_DEBUG_VALUE(X, Y) (X)
#else
#   define MENGINE_WINDOWS_DEBUG_VALUE(X, Y) (Y)
#endif

#if defined(MENGINE_DEBUG)
#   define MENGINE_DEBUG_VALUE(X, Y) (X)
#   define MENGINE_RELEASE_VALUE(X, Y) (Y)
#else
#   define MENGINE_DEBUG_VALUE(X, Y) (Y)
#   define MENGINE_RELEASE_VALUE(X, Y) (X)
#endif

#if defined(MENGINE_MASTER_RELEASE)
#   define MENGINE_MASTER_RELEASE_ENABLE
#else
#   define MENGINE_MASTER_RELEASE_DISABLE
#endif

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
#   define MENGINE_MASTER_RELEASE_VALUE(X, Y) (X)
#else
#   define MENGINE_MASTER_RELEASE_VALUE(X, Y) (Y)
#endif

#if defined(MENGINE_BUILD_PUBLISH)
#   define MENGINE_BUILD_PUBLISH_ENABLE
#else
#   define MENGINE_BUILD_PUBLISH_DISABLE
#endif

#if defined(MENGINE_BUILD_PUBLISH_ENABLE)
#   define MENGINE_BUILD_PUBLISH_VALUE(X, Y) (X)
#else
#   define MENGINE_BUILD_PUBLISH_VALUE(X, Y) (Y)
#endif

#if defined(MENGINE_DEBUG)
#   define MENGINE_DEBUG_ARGUMENTS(...) __VA_ARGS__
#else
#   define MENGINE_DEBUG_ARGUMENTS(...)
#endif

#ifndef MENGINE_ASSERTION_DEBUG
#   if defined(MENGINE_DEBUG)
#       define MENGINE_ASSERTION_DEBUG 1
#   else
#       define MENGINE_ASSERTION_DEBUG 0
#   endif
#endif

#if MENGINE_ASSERTION_DEBUG == 1
#   define MENGINE_ASSERTION_DEBUG_ENABLE
#endif

#ifndef MENGINE_DOCUMENT
#   if defined(MENGINE_DEBUG)
#       define MENGINE_DOCUMENT 1
#   else
#       define MENGINE_DOCUMENT 0
#   endif
#endif

#if MENGINE_DOCUMENT == 1
#   define MENGINE_DOCUMENT_ENABLE
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_VALUE(X, Y) (X)
#else
#   define MENGINE_DOCUMENT_VALUE(X, Y) (Y)
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_ARGUMENTS(...) __VA_ARGS__
#else
#   define MENGINE_DOCUMENT_ARGUMENTS(...)
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_STR(Doc) ((Doc)->getMessage())
#else
#   define MENGINE_DOCUMENT_STR(Doc) MENGINE_STRING_EMPTY
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
#   if defined(MENGINE_WINDOWS_DEBUG)
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

#if defined(MENGINE_PLATFORM_MACOS)
#   define off64_t off_t
#   define fopen64 fopen
#endif

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_COMPILER_GCC) || defined(MENGINE_COMPILER_CLANG)
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

#ifndef MENGINE_LOCALE_LANGUAGE_SIZE
#define MENGINE_LOCALE_LANGUAGE_SIZE 2
#endif

#ifndef MENGINE_MAX_THREAD_NAME
#define MENGINE_MAX_THREAD_NAME 15
#endif

#ifndef MENGINE_MAX_COMMAND_LENGTH
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_MAX_COMMAND_LENGTH 32768
#   else
#       define MENGINE_MAX_COMMAND_LENGTH 1024
#   endif
#endif

#define MENGINE_SHA1_UINT8_COUNT 20
#define MENGINE_SHA1_HEX_COUNT 40

#ifndef MENGINE_PATH_DELIM_BACKSLASH
#define MENGINE_PATH_DELIM_BACKSLASH '/'
#endif

#ifndef MENGINE_PATH_WDELIM_BACKSLASH
#define MENGINE_PATH_WDELIM_BACKSLASH L'/'
#endif

#ifndef MENGINE_PATH_DELIM_FORWARDSLASH
#define MENGINE_PATH_DELIM_FORWARDSLASH '\\'
#endif

#ifndef MENGINE_PATH_WDELIM_FORWARDSLASH
#define MENGINE_PATH_WDELIM_FORWARDSLASH L'\\'
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

#ifndef MENGINE_MAX_ALIGNED_POINTER_VALUE
#   if defined(MENGINE_ENVIRONMENT_ARCHITECTURE_X86)
#       define MENGINE_MAX_ALIGNED_POINTER_VALUE (0xfffffffc)
#   elif defined(MENGINE_ENVIRONMENT_ARCHITECTURE_X64)
#       define MENGINE_MAX_ALIGNED_POINTER_VALUE (0xfffffffffffffff8)
#   else
#       error "invalid define MENGINE_MAX_ALIGNED_POINTER_VALUE [unsupport architecture]"
#   endif
#endif

#ifndef MENGINE_STATIC_STRING_LENGTH
#define MENGINE_STATIC_STRING_LENGTH(S) (sizeof( (S) ) - 1)
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

#ifndef MENGINE_FOURCC
#define MENGINE_FOURCC(c0, c1, c2, c3) (((c0) << 0) | ((c1) << 8) | ((c2) << 16) | ((c3) << 24))
#endif

#ifndef MENGINE_POWER2
#define MENGINE_POWER2(N) (1 << N)
#endif

#ifndef MENGINE_CLAMP
#define MENGINE_CLAMP(A, B, C) MENGINE_MAX( A, MENGINE_MIN(B, C) )
#endif

#ifndef MENGINE_MAX
#define MENGINE_MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MENGINE_MIN
#define MENGINE_MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MENGINE_MIN_MAX
#define MENGINE_MIN_MAX(A, B, C) MENGINE_MIN(A, MENGINE_MAX(B, C))
#endif