#	pragma once

#	include "Interface\StringizeInterface.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	class StringizeService
		: public StringizeServiceInterface
	{
	public:
		ConstString stringize( const String & _str ) override;
	};
}
