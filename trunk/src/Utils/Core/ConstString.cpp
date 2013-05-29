#   include "Config/Typedef.h"

#	include "ConstString.h"

namespace Menge
{
    template<>
    const ConstStringT<char> & ConstStringT<char>::none()
    {
        static ConstStringT<char> s_none( new ConstStringHolderNone<char>() );

        return s_none;
    }
}