#   pragma once

#include "Config/Char.h"

#include "stdex/stl_string.h"
#include "stdex/stl_vector.h"

namespace Mengine
{
    typedef stdex::string String;
    typedef stdex::wstring WString;
	typedef stdex::u32string U32String;

    typedef stdex::vector<Char> TVectorChar;
    typedef stdex::vector<String> TVectorString;
	
    typedef stdex::vector<WChar> TVectorWChar;
    typedef stdex::vector<WString> TVectorWString;

	typedef stdex::vector<U32String> TVectorU32String;
}
