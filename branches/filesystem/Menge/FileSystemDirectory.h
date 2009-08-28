/*
 *	FileSystemDirectory.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileSystem.h"

namespace Menge
{
	class FileSystemDirectory
		: public FileSystem
	{
	public:
		bool initialize( const String& _path, bool _create ) override;
		bool existFile( const String& _filename ) override;
		FileInputInterface* openInputFile( const String& _filename ) override;
		void closeInputFile( FileInputInterface* _file ) override;
		FileOutputInterface* openOutputFile( const String& _filename ) override;
		void closeOutputFile( FileOutputInterface* _outStream ) override;
	};
}	// namespace Menge
