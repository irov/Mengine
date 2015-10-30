#   pragma once

#	include "Config/Char.h"

#	include <stdex/stl_string.h>
#	include <stdex/stl_vector.h>

namespace Menge
{
    typedef stdex::string String;
    typedef stdex::wstring WString;

    typedef stdex::vector<Char> TVectorChar;
    typedef stdex::vector<String> TVectorString;

    typedef stdex::vector<WChar> TVectorWChar;
    typedef stdex::vector<WString> TVectorWString;
}
