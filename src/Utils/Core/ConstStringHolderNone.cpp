#   include "ConstStringHolderNone.h"

namespace Menge
{
    namespace Detail
    {
        template<>
        const char * cs_empty_str<char>()
        {
            return "";
        }
    }
}