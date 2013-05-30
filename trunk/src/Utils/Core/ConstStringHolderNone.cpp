#   include "ConstStringHolderNone.h"

namespace Menge
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<>
        const char * cs_empty_str<char>()
        {
            return "";
        }
        //////////////////////////////////////////////////////////////////////////
        template<>
        const wchar_t * cs_empty_str<wchar_t>()
        {
            return L"";
        }
    }
}