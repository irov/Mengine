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
	class BufferedFileInput;
	class SimpleFileOutput;

	class FileSystemDirectory
		: public FileSystem
	{
	public:
		FileSystemDirectory();
		~FileSystemDirectory();

	public:
		bool initialize( const WString& _path,  FileEngine * _fileEngine, bool _create ) override;
		const WString & getPath() const override;

	public:
		bool existFile( const WString& _filename ) override;
		FileInputStreamInterface* createInputFile() override;
		bool openInputFile( const WString& _filename, FileInputStreamInterface* _file ) override;
		void closeInputFile( FileInputStreamInterface* _inputFile ) override;
		
		FileOutputStreamInterface* createOutputFile() override;

		bool openOutputFile( const WString& _filename, FileOutputStreamInterface* _file ) override;
		void closeOutputFile( FileOutputStreamInterface* _outputFile ) override;
		bool createDirectory( const WString& _path ) override;
		void removeDirectory( const WString& _path ) override;
		void removeFile( const WString& _filename ) override;

	public:
		FileBufferProvider * getBufferProvider() const override;

	private:
		void makeFullname_( const WString& _filename, WString& _fullname );

	private:
		WString m_path;

		WString m_fullnameCache;

		FileSystemInterface* m_interface;
		FileEngine * m_fileEngine;

		//typedef Pool<BufferedFileInput, PoolPlacementPolicyErase> TFileInputPool;
		//TFileInputPool m_fileInputPool;

		//typedef Pool<SimpleFileOutput, PoolPlacementPolicyErase> TFileOutputPool;
		//TFileOutputPool m_fileOutputPool;

		FileBufferProvider * m_fileBufferProvider;
	};
}	// namespace Menge
