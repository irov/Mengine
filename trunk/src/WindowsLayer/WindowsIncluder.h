#	pragma once

#ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#pragma warning(push, 0) 
#	include <Windows.h>
#   include <WinUser.h>

#   include <shlobj.h>
#pragma warning(pop) 
