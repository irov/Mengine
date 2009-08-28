#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class FileInputInterface;

	namespace Utils
	{
		const String& emptyString();
		TStringVector split( const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void trim( String& str, bool left = true, bool right = true );
		String toString( int x );
		String toString( std::size_t _x );
		StringA WToA( const StringW & _value );
		StringW AToW( const StringA & _value );
		void skip( FileInputInterface* _file, int _count );
		bool eof( FileInputInterface* _file );
		String getLine( FileInputInterface* _file,  bool _trimAfter = true );
		int skipLine( FileInputInterface* _file, const String& _delim = "\n" );
		bool getFileExt( String & _out, const String & _filename );
	}
};
