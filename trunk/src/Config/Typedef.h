#	pragma once

#	ifdef _MSC_VER
#	pragma warning(disable:4250) // remove inherits dominance
#	endif

#	include "Config.h"

#	include <string>
#	include <vector>
#	include <map>
#	include <list>
#	include <iostream>
#	include <cstring>
#	include <cstdlib>
#	include <sstream>
//#	include <string.h>
//#	include <iosfwd>

namespace Menge
{
	typedef wchar_t WChar;
	typedef char Char;

    typedef std::string String;
    typedef std::wstring WString;
        
    typedef std::stringstream Stringstream;
    typedef std::wstringstream WStringstream;

    //typedef String FilePath; //UTF8
    //typedef ConstString FilePath; //UTF8

//#ifdef MENGE_UNICODE
//	typedef TCharW TChar;
//	typedef WString String;
//	typedef StringstreamW Stringstream;
//	typedef std::wofstream StdOfstream;
//
//#	define StdErr (std::wcerr)
//#	define StdStrchr (std::wcschr)
//#	define STDVSPRINTF (vswprintf_s)
//#	define STDSPRINTF (swprintf_s)
//#	define STDSSCANF (swscanf_s)
//#	define STDSTRCPY (wcscpy)
//#	define STDSTRCMP (wcscmp)
//#	define STDSTRLEN (wcslen)
//#	define STDITOS (_itow)
//
//
//#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
//#		define MENGE_TEXT(quote)	L##quote
//#	endif
//
//#	else
//	typedef TCharA TChar;
//	typedef AString String;
//	typedef StringstreamA Stringstream;
//	typedef std::ofstream StdOfstream;
//
//#	define StdErr (std::cerr)
//#	define StdStrchr (std::strchr)
//#	define STDVSPRINTF (vsprintf_s)
//#	define STDSPRINTF (sprintf_s)
//#	define STDSSCANF (sscanf_s)
//#	define STDSTRCPY (strcpy)
//#	define STDSTRCMP (strcmp)
//#	define STDSTRLEN (strlen)
//#	define STDITOS (_itoa)
//
////#	if MENGE_COMPILER == MENGE_COMPILER_MSVC
//#		define MENGE_TEXT(quote)	L##quote
////#	endif
//
//#endif

	//typedef _StringBase String;

	typedef void* WindowHandle;

	//

#if MENGE_COMPILER == MENGE_COMPILER_MSVC
	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;
#else
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
#endif
	
	typedef std::vector<unsigned char> TBlobject;
	typedef std::vector<String> TVectorString;
	typedef std::vector<WString> TVectorWString;    
	typedef std::map<WString, WString> TMapWString;
}

//class DataStreamInterface;
//typedef DataStreamInterface FileDataInterface;
//typedef DataStreamInterface MemoryDataInterface;
