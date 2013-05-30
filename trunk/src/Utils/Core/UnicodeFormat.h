#   pragma once

#   include "Config/Typedef.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4800)  
#   endif

#	include <boost/format.hpp>

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef boost::basic_format<Char> StringFormat;
    typedef boost::basic_format<WChar> UnicodeFormat;
}