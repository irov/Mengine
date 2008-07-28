#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	const String& emptyString();
	TStringVector split( const String& _str, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
	String toString( int x );
};