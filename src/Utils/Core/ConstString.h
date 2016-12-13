#   pragma once

#   include "stdex/const_string2.h"

namespace Menge
{
    typedef stdex::const_string2 ConstString;

	//typedef stdex::vector<ConstString> TVectorConstString;
} 

namespace std
{
	template<> inline void swap( Menge::ConstString & _left, Menge::ConstString & _right )
	{
		_left.swap( _right );
	}
}