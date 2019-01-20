#pragma once

#ifdef WINVER
#undef WINVER
#define WINVER 0x0600
#endif

#ifdef _WIN32_WINNT	
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0600
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
#include <synchapi.h>
#pragma warning(pop) 
