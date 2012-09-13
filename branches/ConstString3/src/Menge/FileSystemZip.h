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
	class FileInputMemory;
	class FileSystemInterface;
	class MappedFileInputStreamInterface;

	class FileSystemZip
		: public FileGroupInterface
	{
	public:
		FileSystemZip();
		~FileSystemZip();

	public:
		bool initialize( const WString& _path, FileEngine * _fileEngine, bool _create ) override;
		const WString & getPath() const override;

	public:
		bool existFile( const WString& _filename ) override;

    public:
		FileInputStreamInterface * createInputFile() override;
		bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) override;
		void closeInputFile( FileInputStreamInterface* _file ) override;

    public:
        FileOutputStreamInterface* createOutputFile() override;
        bool openOutputFile( const WString& _filename, FileOutputStreamInterface* _file ) override;
        void closeOutputFile( FileOutputStreamInterface* _outStream ) override;

    public:
        bool createDirectory( const WString& _path ) override;
        bool removeDirectory( const WString& _path ) override;
        bool removeFile( const WString& _filename ) override;

	private:
		WString m_path;
		FileEngine * m_fileEngine;
		FileSystemInterface* m_interface;
		MappedFileInputStreamInterface* m_zipFile;
		
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
