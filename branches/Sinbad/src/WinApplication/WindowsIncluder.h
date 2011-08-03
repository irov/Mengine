#	pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
//#define _WIN32_WINNT 0x0410	// Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINNT 0x0500	// allow AttachConsole
#endif




#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

//#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
