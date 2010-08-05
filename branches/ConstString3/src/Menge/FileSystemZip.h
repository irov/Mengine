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
#	include "Core/Pool.h"

namespace Menge
{
	class MemoryFileInput;

	class FileSystemZip
		: public FileSystem
	{
	public:
		FileSystemZip();
		~FileSystemZip();

	public:
		bool initialize( const String& _path, FileEngine * _fileEngine, bool _create ) override;
		const String & getPath() const override;

	public:
		bool existFile( const String& _filename ) override;
		FileInputInterface * createInputFile() override;
		bool openInputFile( const String& _filename, FileInputInterface* _file ) override;
		void closeInputFile( FileInputInterface* _file ) override;

	private:
		String m_path;
		FileEngine * m_fileEngine;

		MemoryFileInput* m_zipFile;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef std::map<String, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;

		typedef Pool<MemoryFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;
	};
}	// namespace Menge
