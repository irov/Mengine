/*
 *	FileSystem.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Factorable.h"

namespace Menge
{
	class FileInput;
	class FileOutput;

	class FileSystem
		: public Factorable
	{
	public:
		virtual bool initialize( const String& _path, bool _create ) = 0;
		virtual bool existFile( const String& _filename ) = 0;
		virtual FileInput* createInputFile() = 0;
		virtual bool openInputFile( const String& _filename, FileInput* _file ) = 0;
		virtual void closeInputFile( FileInput* _file ) = 0;

		virtual FileOutput* createOutputFile() { return 0; }
		virtual bool openOutputFile( const String& _filename, FileOutput* _file ) { return false; }
		virtual void closeOutputFile( FileOutput* _outStream ) { }
		virtual bool createDirectory( const String& _path ) { return false; }
		virtual void removeDirectory( const String& _path ) { }
		virtual void removeFile( const String& _filename ) { }
	};
}	// namespace Menge
