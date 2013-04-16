/*
 *	PosixFileSystem.h
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class PosixFileSystem
		: public FileSystemInterface
	{
	public:
		PosixFileSystem();
		~PosixFileSystem();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider ) override;

    public:
		bool existFile( const String& _filename ) const override;

    public:
        FileInputStreamInterface* createInputStream() override;		
        FileOutputStreamInterface* createOutputStream() override;

    public:
        MappedFileInputStreamInterface * createMappedInputStream();

    public:
        bool deleteFile( const FilePath & _filename ) override;
        bool createFolder( const FilePath & _path ) override;
        bool deleteFolder( const FilePath & _path ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;
	};
}	// namespace Menge
