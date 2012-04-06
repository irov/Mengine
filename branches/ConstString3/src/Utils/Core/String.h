#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{	
	typedef std::vector<ConstString> TVectorConstString;

	namespace Utils
	{
		const String& emptyString();
		const WString& emptyWString();
		const ConstString& emptyConstString();

		
		void split( TVectorString & _outStrings, const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void wsplit( TVectorWString & _outStrings, const WString& _str, bool _trimDelims, const WString& _delims = L"\t\n ", unsigned int _maxSplits = 0 );

		void join( const String& _delim, const TVectorString& _stringArray, String & _outString );
		void trim( String& str, bool left = true, bool right = true );

		String toString( int x );
		String toString( unsigned int _x );

		WString toWString( int x );
		WString toWString( unsigned int _x );

		String WToA( const WString & _value );
		WString AToW( const String & _value );
	}
};
