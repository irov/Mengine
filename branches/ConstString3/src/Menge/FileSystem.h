/*
 *	FileSystem.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Factory/Factorable.h"

namespace Menge
{
	class FileInputInterface;
	class FileOutputInterface;

	class FileSystem
		: public Factorable
	{
	public:
		virtual bool initialize( const ConstString& _path, bool _create ) = 0;
		virtual bool existFile( const ConstString& _filename ) = 0;
		virtual FileInputInterface* createInputFile() = 0;
		virtual bool openInputFile( const ConstString& _filename, FileInputInterface* _file ) = 0;
		virtual void closeInputFile( FileInputInterface* _file ) = 0;

		virtual FileOutputInterface* createOutputFile() { return 0; }
		virtual bool openOutputFile( const ConstString& _filename, FileOutputInterface* _file ) { return false; }
		virtual void closeOutputFile( FileOutputInterface* _outStream ) { }
		virtual bool createDirectory( const ConstString& _path ) { return false; }
		virtual void removeDirectory( const ConstString& _path ) { }
		virtual void removeFile( const ConstString& _filename ) { }
	};
}	// namespace Menge
