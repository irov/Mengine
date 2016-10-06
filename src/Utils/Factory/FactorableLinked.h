#	pragma once

#   include "Factory/Factorable.h"

#   include "stdex/intrusive_linked.h"

//typedef stdex::intrusive_ptr<Type> TypePtr;

namespace Menge
{
	class FactorableLinked
        : public Factorable
        , public stdex::intrusive_linked<FactorableLinked>
	{
	};
}
