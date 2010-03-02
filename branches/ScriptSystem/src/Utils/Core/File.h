#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class FileInputInterface;
	class FileOutputInterface;

	namespace Utils
	{
		void skip( FileInputInterface* _file, int _count );
		bool eof( FileInputInterface* _file );

		String getLine( FileInputInterface* _file,  bool _trimAfter = true );
		int skipLine( FileInputInterface* _file, const String& _delim = "\n" );
		void fileWrite( FileOutputInterface* _file, const String& _string );

		bool getFileExt( String & _out, const String & _filename );
		void collapsePath( const String& _path, String& _collapsedPath );
	}
};
