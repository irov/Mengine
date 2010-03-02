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

		bool initialize( const String& _path, bool _create ) override;
		bool existFile( const String& _filename ) override;
		FileInput* createInputFile() override;
		bool openInputFile( const String& _filename, FileInput* _file ) override;
		void closeInputFile( FileInput* _file ) override;

	private:
		void makeFullname_( const String& _filename, String* _fullname );
		void closeMemFile_( MemoryFileInput* _file );

	private:
		String m_path;

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
