#	pragma once

#   include "Factory/FactorablePtr.h"

#   include "stdex/intrusive_linked.h"

//typedef stdex::intrusive_ptr<Type> TypePtr;

namespace Menge
{
	class FactorableLinkedPtr
        : public FactorablePtr
        , public stdex::intrusive_linked<FactorableLinkedPtr>
	{
	};
}
