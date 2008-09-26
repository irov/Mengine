#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DataStreamInterface;
	namespace Utils
	{
		const String& emptyString();
		TStringVector split( const String& _str, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void trim( String& str, bool left = true, bool right = true );
		String toString( int x );
		StringA WToA( const StringW& _stringw );
		StringW AToW( const StringA& _stringa );
		String getLine( DataStreamInterface* _stream,  bool _trimAfter = true );
		std::streamsize skipLine( DataStreamInterface* _stream, const String& _delim = "\n" );
	}
};