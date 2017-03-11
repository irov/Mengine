#   pragma once

#   include "Factory/Factorable.h"

#   include "stdex/intrusive_linked.h"
#	include "stdex/intrusive_ptr.h"
#   include "stdex/const_string2_holder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    class ConstStringHolder
        : public Factorable
        , public stdex::intrusive_linked<ConstStringHolder>
        , public stdex::const_string2_holder
    {
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ConstStringHolder> ConstStringHolderPtr;
	//////////////////////////////////////////////////////////////////////////
} 