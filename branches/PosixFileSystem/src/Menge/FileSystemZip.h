/*
 *	FileSystemZip.h
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileSystem.h"
#	include <map>
#	include "Pool.h"

namespace Menge
{
	class MemoryFileInput;

	class FileSystemZip
		: public FileSystem
	{
	public:
		FileSystemZip();
		~FileSystemZip();

		bool initialize( const String& _path, bool _create ) override;
		bool existFile( const String& _filename ) override;
		FileInput* createInputFile() override;
		bool openInputFile( const String& _filename, FileInput* _file ) override;
		void closeInputFile( FileInput* _file ) override;

	private:
		String m_path;
		MemoryFileInput* m_zipFile;

		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef std::map< String, FileInfo > TFileInfoMap;
		TFileInfoMap m_files;

		typedef Pool<MemoryFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;
	};
}	// namespace Menge
