#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class FileInputInterface;
	class FileOutputInterface;

	namespace Utils
	{
		const String& emptyString();
		TStringVector split( const String& _str, bool _trimDelims, const String& _delims = "\t\n ", unsigned int _maxSplits = 0 );
		void join( const String& _delim, const TStringVector& _stringArray, String* _outString );
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
		void fileWrite( FileOutputInterface* _file, const String& _string );
		void collapsePath( const String& _path, String* _collapsedPath );
	}
};
