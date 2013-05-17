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

	typedef void* WindowHandle;
    
#   ifdef MENGE_COMPILER_MSVC
	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;
#   else
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
#   endif
	
	typedef std::vector<unsigned char> TBlobject;
    
    typedef std::vector<Char> TVectorChar;
	typedef std::vector<String> TVectorString;

    typedef std::vector<WChar> TVectorWChar;
	typedef std::vector<WString> TVectorWString;

	typedef std::map<WString, WString> TMapWString;
}
