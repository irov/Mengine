#pragma once

#include "Config/Config.h"

#ifndef MENGINE_WINDOWS_VERSION
    #ifdef MENGINE_TOOLCHAIN_MINGW
        #define MENGINE_WINDOWS_VERSION 0x0501
    #else
        #define MENGINE_WINDOWS_VERSION 0x0600
    #endif
#endif

#ifdef WINVER
#undef WINVER
#define WINVER MENGINE_WINDOWS_VERSION
#endif

#ifdef _WIN32_WINNT	
#undef _WIN32_WINNT
#define _WIN32_WINNT MENGINE_WINDOWS_VERSION
#endif

#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#define _WIN32_WINDOWS MENGINE_WINDOWS_VERSION
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
#include <Shlwapi.h>

#include <tlhelp32.h>
#include <shellapi.h>
#include <shlobj.h>
#pragma warning(pop) 
