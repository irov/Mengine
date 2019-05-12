#pragma once

#ifndef MENGINE_MAX_PATH
#define MENGINE_MAX_PATH 260
#endif

#ifndef MENGINE_UNUSED
#define MENGINE_UNUSED(X) ((void)X)
#endif

#ifndef NDEBUG
#define MENGINE_DEBUG
#endif

#if defined(WIN32)
#define MENGINE_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(MACOSX)
#define MENGINE_PLATFORM_MACOS
#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define MENGINE_PLATFORM_IOS
#elif defined(__ANDROID__)
#define MENGINE_PLATFORM_ANDROID
#else
#error "undefine platform"
#endif

#if defined(__MINGW32__)
#define MENGINE_TOOLCHAIN_MINGW
#elif defined(_MSC_VER)
#define MENGINE_TOOLCHAIN_MSVC
#endif

#if defined(MENGINE_PLATFORM_IOS) || defined(MENGINE_PLATFORM_ANDROID)
#define MENGINE_PLATFORM_MOBILE
#endif

#ifndef NDEBUG
#if defined(MENGINE_PLATFORM_WINDOWS)
#define MENGINE_WINDOWS_DEBUG
#endif
#endif