#   pragma once

#   include "Config/Typedef.h"

#pragma warning(push, 0) 
#	include <boost/format.hpp>
#pragma warning(pop) 

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef boost::basic_format<WChar> UnicodeFormat;
}