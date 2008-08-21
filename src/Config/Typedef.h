#	pragma once

#	include "Config.h"

#	include <string>
#	include <vector>
#	include <list>
#	include <iostream>
//#	include <string.h>

namespace Menge
{

typedef wchar_t TCharW;
typedef char	TCharA;
typedef std::wstring StringW;
typedef std::string StringA;

#	if MENGE_WCHAR_T_STRINGS
		typedef TCharW TChar;
		typedef StringW String;
		#define StdErr (std::wcerr)
		#define StdStrchr (std::wcschr)
		typedef std::wofstream StdOfstream;

		#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
			#	define MENGE_TEXT(quote)	L##quote
		#	endif

#	else
		typedef TCharA TChar;
		typedef StringA String;
		#define StdErr (std::cerr)
		typedef std::ofstream StdOfstream;
		#define StdStrchr (std::strchr)

		#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
			#	define MENGE_TEXT(quote)	quote
		#	endif

#	endif

//typedef _StringBase String;

typedef void* WindowHandle;

//

#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
		typedef unsigned __int64 uint64;
		typedef unsigned __int32 uint32;
		typedef unsigned __int16 uint16;
		typedef unsigned __int8 uint8;
#	else
		typedef unsigned long long uint64;
		typedef unsigned int uint32;
		typedef unsigned short uint16;
		typedef unsigned char uint8;
#	endif

}
typedef std::vector<Menge::String> TStringVector;
//class DataStreamInterface;
//typedef DataStreamInterface FileDataInterface;
//typedef DataStreamInterface MemoryDataInterface;