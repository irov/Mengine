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
		virtual bool initialize( const WString& _path, FileEngine * _fileEngine, bool _create ) = 0;
		virtual const WString & getPath() const = 0;

	public:
		virtual bool existFile( const WString& _filename ) = 0;
		virtual FileInputStreamInterface* createInputFile() = 0;
		virtual bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) = 0;
		virtual void closeInputFile( FileInputStreamInterface* _file ) = 0;
		

	public:
		virtual FileBufferProvider * getBufferProvider() const = 0;

	public:
		virtual FileOutputStreamInterface* createOutputFile() { return 0; }
		virtual bool openOutputFile( const WString& _filename, FileOutputStreamInterface* _file ) { return false; }
		virtual void closeOutputFile( FileOutputStreamInterface* _outStream ) { }
		virtual bool createDirectory( const WString& _path ) { return false; }
		virtual void removeDirectory( const WString& _path ) { }
		virtual void removeFile( const WString& _filename ) { }
	};
}	// namespace Menge
