#pragma once

#ifndef MENGINE_MAX_PATH
#define MENGINE_MAX_PATH 260
#endif

#define MENGINE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MENGINE_MIN(a, b) ((a) < (b) ? (a) : (b))

#define MENGINE_STRING_EMPTY ""

#define MENGINE_PP_STRINGIZE_I(X) #X
#define MENGINE_PP_STRINGIZE(X) MENGINE_PP_STRINGIZE_I(X)

#define MENGINE_PP_CONCATENATE_I(X, Y) X ## Y
#define MENGINE_PP_CONCATENATE(X, Y) MENGINE_PP_CONCATENATE_I(X, Y)

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)X)
#endif

#ifdef __GNUC__
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
#endif

#ifdef MENGINE_DEBUG
#define MENGINE_DEBUG_ATTRIBUTE(X, Y) (X)
#else
#define MENGINE_DEBUG_ATTRIBUTE(X, Y) (Y)
#endif

#define MENGINE_CONSTEXPR constexpr
#define MENGINE_INLINE inline

#if defined(WIN32)
#   define MENGINE_PLATFORM_WINDOWS
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__linux__)
#   define MENGINE_PLATFORM_LINUX
#   define MENGINE_PLATFORM_DESKTOP
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if defined(TARGET_OS_OSX)
#       define MENGINE_PLATFORM_OSX
#       define MENGINE_PLATFORM_DESKTOP
#   endif
#   if defined(TARGET_OS_IPHONE)
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

#if defined(MENGINE_PLATFORM_IOS) || defined(MENGINE_PLATFORM_ANDROID)
#   define MENGINE_PLATFORM_MOBILE
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