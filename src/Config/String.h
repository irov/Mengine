#   pragma once

#	include <stdex/stl_string.h>
#	include <stdex/stl_vector.h>

namespace Menge
{
    typedef wchar_t WChar;
    typedef char Char;

    typedef stdex::string String;
    typedef stdex::wstring WString;

    typedef stdex::vector<Char> TVectorChar;
    typedef stdex::vector<String> TVectorString;

    typedef stdex::vector<WChar> TVectorWChar;
    typedef stdex::vector<WString> TVectorWString;
}
