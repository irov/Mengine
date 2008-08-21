#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	namespace Utils
	{
		const String& emptyString();
		TStringVector split( const String& _str, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		String toString( int x );
		StringA WToA( const StringW& _stringw );
		StringW AToW( const StringA& _stringa );
	}
};