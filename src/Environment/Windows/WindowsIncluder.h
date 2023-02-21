#pragma once

#include "Config/Config.h"

#ifndef MENGINE_PLATFORM_WINDOWS
#error "include only for 'Windows' platform"
#endif

#include <sdkddkver.h>

#ifndef MENGINE_WINDOWS_MIN_VERSION
#   if defined(MENGINE_WINDOWS_MIN_VERSION_WINXP)
#       define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_WINXP
#   elif defined(MENGINE_WINDOWS_MIN_VERSION_VISTA)
#       define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_VISTA
#   elif defined(MENGINE_WINDOWS_MIN_VERSION_WIN7)
#       define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_WIN7
#   elif defined(MENGINE_WINDOWS_MIN_VERSION_WIN8)
#       define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_WIN8
#   elif defined(MENGINE_WINDOWS_MIN_VERSION_WIN10)
#       define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_WIN10
#   else
#       if defined(MENGINE_TOOLCHAIN_MINGW)
#           define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_VISTA
#       elif defined(MENGINE_PLATFORM_UWP)
#           define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_WIN10
#       else
#           define MENGINE_WINDOWS_MIN_VERSION _WIN32_WINNT_VISTA
#       endif
#   endif
#endif

#if MENGINE_WINDOWS_MIN_VERSION >= _WIN32_WINNT_WINXP
#   define MENGINE_WINDOWS_SUPPORT_MIN_VERSION_WINXP
#endif

#if MENGINE_WINDOWS_MIN_VERSION >= _WIN32_WINNT_VISTA
#   define MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
#endif

#if MENGINE_WINDOWS_MIN_VERSION >= _WIN32_WINNT_WIN7
#   define MENGINE_WINDOWS_SUPPORT_MIN_VERSION_WIN7
#endif

#if MENGINE_WINDOWS_MIN_VERSION >= _WIN32_WINNT_WIN8
#   define MENGINE_WINDOWS_SUPPORT_MIN_VERSION_WIN8
#endif

#if MENGINE_WINDOWS_MIN_VERSION >= _WIN32_WINNT_WIN10
#   define MENGINE_WINDOWS_SUPPORT_MIN_VERSION_WIN10
#endif

#ifdef WINVER
#undef WINVER
#define WINVER MENGINE_WINDOWS_MIN_VERSION
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT MENGINE_WINDOWS_MIN_VERSION
#endif

#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#define _WIN32_WINDOWS MENGINE_WINDOWS_MIN_VERSION
#endif

#define WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif

#pragma warning(push, 0) 
#include <Windows.h>
#include <WinUser.h>
#include <WinBase.h>

#include <Psapi.h>
#include <shlwapi.h>
#include <Lmcons.h>

#include <tlhelp32.h>
#include <shellapi.h>
#include <shlobj.h>

#include <windowsx.h>
#include <wtsapi32.h>
#include <libloaderapi.h>
#pragma warning(pop) 
