/*
 *	FileSystemDirectory.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileSystem.h"
#	include "Core/Pool.h"

namespace Menge
{
	class FileSystemInterface;

	class FileSystemDirectory
		: public FileGroupInterface
	{
	public:
		FileSystemDirectory();
		~FileSystemDirectory();

	public:
		bool initialize( const WString& _path,  FileEngine * _fileEngine, bool _create ) override;
		const WString & getPath() const override;

	public:
		bool existFile( const WString& _filename ) override;

    public:
		FileInputStreamInterface* createInputFile() override;
		bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) override;
		void closeInputFile( FileInputStreamInterface* _inputFile ) override;
		
    public:
		FileOutputStreamInterface* createOutputFile() override;        
		bool openOutputFile( const WString& _filename, FileOutputStreamInterface* _file ) override;
		void closeOutputFile( FileOutputStreamInterface* _outputFile ) override;

    public:
		bool createDirectory( const WString& _path ) override;
		bool removeDirectory( const WString& _path ) override;
		bool removeFile( const WString& _filename ) override;

	private:
		void makeFullname_( const WString& _filename, WString& _fullname );

	private:
		WString m_path;

		WString m_fullnameCache;

		FileSystemInterface* m_interface;
		FileEngine * m_fileEngine;
	};
}	// namespace Menge
