#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class InputStreamInterface;
	class OutputStreamInterface;

	namespace Utils
	{
		void skip( InputStreamInterface * _stream, int _count );
		bool eof( InputStreamInterface * _stream );

		String getLine( InputStreamInterface * _stream,  bool _trimAfter = true );
		int skipLine( InputStreamInterface * _stream, const String& _delim = "\n" );
		void stringWrite( OutputStreamInterface * _stream, const String& _string );

		bool getFileExt( String & _out, const ConstString & _filename );
		bool getFileExt( String & _out, const String & _filename );
		void collapsePath( const String& _path, String& _collapsedPath );
	}
};
