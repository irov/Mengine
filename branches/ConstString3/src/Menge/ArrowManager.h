#	pragma once

#	include "PrototypeManager.h"

#	include "Core/Holder.h"

namespace Menge
{
	class Arrow;

	class ArrowManager
		: public PrototypeManager
		, public Holder<ArrowManager>
	{
	public:
		Arrow * createArrow( const ConstString & _prototype );
	};
}