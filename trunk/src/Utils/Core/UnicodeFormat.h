#   pragma once

#   include "Config/Typedef.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   endif

#	include <boost/format.hpp>

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef boost::basic_format<WChar> UnicodeFormat;
}