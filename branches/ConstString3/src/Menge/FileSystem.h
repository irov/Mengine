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
#	include "Core/ConstString.h"

namespace Menge
{
	class FileInputInterface;
	class FileOutputInterface;

	class FileEngine;

	class FileSystem
		: public Factorable
	{
	public:
		virtual bool initialize( const String& _path, FileEngine * _fileEngine, bool _create ) = 0;
		virtual bool existFile( const String& _filename ) = 0;
		virtual FileInputInterface* createInputFile() = 0;
		virtual bool openInputFile( const String& _filename, FileInputInterface* _file ) = 0;
		virtual void closeInputFile( FileInputInterface* _file ) = 0;

		virtual FileOutputInterface* createOutputFile() { return 0; }
		virtual bool openOutputFile( const String& _filename, FileOutputInterface* _file ) { return false; }
		virtual void closeOutputFile( FileOutputInterface* _outStream ) { }
		virtual bool createDirectory( const String& _path ) { return false; }
		virtual void removeDirectory( const String& _path ) { }
		virtual void removeFile( const String& _filename ) { }
	};
}	// namespace Menge
