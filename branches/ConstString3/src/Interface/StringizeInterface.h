#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class StringizeServiceInterface
		: public ServiceInterface
	{
	public:
		virtual ConstString stringize( const String & _str ) = 0;
	};
}
