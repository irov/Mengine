#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	namespace Utils
	{
		const String& emptyString();

		void split( TVectorString & _outStrings, const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void join( const String& _delim, const TVectorString& _stringArray, String & _outString );
		void trim( String& str, bool left = true, bool right = true );

		String toString( int x );
		String toString( std::size_t _x );

		StringA WToA( const StringW & _value );
		StringW AToW( const StringA & _value );
	}
};
