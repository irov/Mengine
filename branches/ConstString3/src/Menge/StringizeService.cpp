#	include "StringizeService.h"


namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	ConstString StringizeService::stringize( const String & _str )
	{
		return ConstString(_str);
	}
	
}
