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

	class FileEngine;

	class FileGroupInterface
		: public Factorable
	{
	public:
		virtual bool initialize( const WString& _path, FileEngine * _fileEngine, bool _create ) = 0;
		virtual const WString & getPath() const = 0;

	public:
		virtual bool existFile( const WString& _filename ) = 0;

    public:
		virtual FileInputStreamInterface* createInputFile() = 0;
		virtual bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) = 0;
		virtual void closeInputFile( FileInputStreamInterface* _file ) = 0;
		
	public:
		virtual FileOutputStreamInterface* createOutputFile() = 0;
		virtual bool openOutputFile( const WString& _filename, FileOutputStreamInterface* _file ) = 0;
		virtual void closeOutputFile( FileOutputStreamInterface* _outStream ) = 0;
		
    public:
        virtual bool createDirectory( const WString& _path ) = 0;
		virtual bool removeDirectory( const WString& _path ) = 0;
		virtual bool removeFile( const WString& _filename ) = 0;
	};
}	// namespace Menge
