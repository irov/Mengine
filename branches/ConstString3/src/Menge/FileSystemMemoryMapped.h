/*
 *	FileSystemMemoryMapped.h
 *
 *	Created by _Berserk_ on 8.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileSystem.h"
#	include <map>
#	include "Core/Pool.h"

namespace Menge
{
	class MemoryFileInput;

	class FileSystemMemoryMapped
		: public FileSystem
	{
	public:
		FileSystemMemoryMapped();
		~FileSystemMemoryMapped();

		bool initialize( const ConstString& _path, FileEngine * _fileEngine, bool _create ) override;

	public:
		bool existFile( const ConstString& _filename ) override;
		FileInputInterface* createInputFile() override;
		bool openInputFile( const ConstString& _filename, FileInputInterface * _file ) override;
		void closeInputFile( FileInputInterface * _file ) override;

	private:
		void makeFullname_( const ConstString& _filename, String& _fullname );
		void closeMemFile_( MemoryFileInput* _file );

	private:
		String m_path;
		FileEngine * m_fileEngine;

		struct FileInfo
		{
			void* pMem;
			int size;
			int refCount;
		};

		typedef std::map<String, FileInfo> TMappedFilesMap;
		TMappedFilesMap m_files;
		typedef std::map<MemoryFileInput*, String> TMemFileMap;
		TMemFileMap m_memFileMap;

		typedef Pool<MemoryFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;
	};
}	// namespace Menge
