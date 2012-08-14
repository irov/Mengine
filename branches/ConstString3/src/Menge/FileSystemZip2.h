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
	class FileSystemInterface;
	class MappedFileInputStreamInterface;

	class FileSystemZip2
		: public FileGroupInterface
	{
	public:
		FileSystemZip2();
		~FileSystemZip2();

	public:
		bool initialize( const WString& _path, FileEngine * _fileEngine, bool _create ) override;
		const WString & getPath() const override;

	public:
		bool existFile( const WString& _filename ) override;
		FileInputStreamInterface * createInputFile() override;
		bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) override;
		void closeInputFile( FileInputStreamInterface* _file ) override;

	private:
		WString m_path;
		FileEngine * m_fileEngine;
		FileSystemInterface* m_interface;
		FileInputStreamInterface* m_zipFile;
		
		struct FileInfo
		{
			size_t seek_pos;
			size_t file_size;
			size_t unz_size;
			uint16 compr_method;
		};

		typedef std::map<WString, FileInfo> TMapFileInfo;
		TMapFileInfo m_files;

		//typedef Pool<MemoryFileInput, PoolPlacementPolicyNone> TFileInputPool;
		//TFileInputPool m_fileInputPool;
	};
}	// namespace Menge
