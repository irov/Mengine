#   pragma once

#include "stdex/const_string2.h"

namespace Mengine
{
    typedef stdex::const_string2 ConstString;

	//typedef stdex::vector<ConstString> TVectorConstString;
} 

namespace std
{
	inline void swap( Mengine::ConstString & _left, Mengine::ConstString & _right )
	{
		_left.swap( _right );
	}
}
