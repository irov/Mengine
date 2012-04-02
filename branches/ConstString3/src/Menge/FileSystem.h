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
	class FileInputStreamInterface;
	class FileOutputStreamInterface;
	class FileBufferProvider;

	class FileEngine;

	class FileSystem
		: public Factorable
	{
	public:
		virtual bool initialize( const String& _path, FileEngine * _fileEngine, bool _create ) = 0;
		virtual const String & getPath() const = 0;

	public:
		virtual bool existFile( const String& _filename ) = 0;
		virtual FileInputStreamInterface* createInputFile() = 0;
		virtual bool openInputFile( const String& _filename, FileInputStreamInterface* _file ) = 0;
		virtual void closeInputFile( FileInputStreamInterface* _file ) = 0;
		

	public:
		virtual FileBufferProvider * getBufferProvider() const = 0;

	public:
		virtual FileOutputStreamInterface* createOutputFile() { return 0; }
		virtual bool openOutputFile( const String& _filename, FileOutputStreamInterface* _file ) { return false; }
		virtual void closeOutputFile( FileOutputStreamInterface* _outStream ) { }
		virtual bool createDirectory( const String& _path ) { return false; }
		virtual void removeDirectory( const String& _path ) { }
		virtual void removeFile( const String& _filename ) { }
	};
}	// namespace Menge
