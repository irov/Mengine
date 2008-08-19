#	pragma once

#	include "Config.h"

#	include <string>
#	include <vector>
#	include <list>
#	include <iostream>

namespace Menge
{

#	if MENGE_WCHAR_T_STRINGS
		typedef wchar_t TChar;
		typedef std::wstring _StringBase;
		#define StdErr (std::wcerr)
		typedef std::wofstream StdOfstream;

		#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
			#	define MENGE_TEXT(quote)	L##quote
		#	endif

#	else
		typedef char TChar;
		typedef std::string _StringBase;
		#define StdErr (std::cerr)
		typedef std::ofstream StdOfstream;

		#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
			#	define MENGE_TEXT(quote)	quote
		#	endif

#	endif

typedef _StringBase String;

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