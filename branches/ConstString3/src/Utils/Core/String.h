#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{
	typedef std::vector<Menge::String> TVectorString;
	typedef std::vector<ConstString> TVectorConstString;

	namespace Utils
	{
		const String& emptyString();
		const ConstString& emptyConstString();

		
		void split( TVectorString & _outStrings, const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void join( const String& _delim, const TVectorString& _stringArray, String & _outString );
		void trim( String& str, bool left = true, bool right = true );

		String toString( int x );
		String toString( unsigned int _x );

		AString WToA( const WString & _value );
		WString AToW( const AString & _value );
	}
};
