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
    extern char g_resDirectory[ 1024 ];
    
	class PosixInputStream;

	class PosixFileSystem
		: public FileSystemInterface
	{
	public:
		PosixFileSystem();
		~PosixFileSystem();

		bool existFile( const String& _filename ) override;
		InputStreamInterface* openInputStream( const String& _filename ) override;
		void closeInputStream( InputStreamInterface* _stream ) override;
		OutputStreamInterface* openOutputStream( const String& _filename ) override;
		void closeOutputStream( OutputStreamInterface* _stream ) override;
		void* openMappedFile( const String& _filename, int* _size ) override;
		void closeMappedFile( void* _file ) override;
		bool deleteFile( const String& _filename ) override;
		bool createFolder( const String& _path ) override;
		bool deleteFolder( const String& _path ) override;

	private:
		typedef std::vector<PosixInputStream*> TInputStreamPool;
		TInputStreamPool m_inputStreamPool;

		struct FileMappingInfo
		{
			int hFile;
			int size;
		};
		typedef std::map< void*, FileMappingInfo > TFileMappingMap;

		TFileMappingMap m_fileMappingMap;
	};
}	// namespace Menge
