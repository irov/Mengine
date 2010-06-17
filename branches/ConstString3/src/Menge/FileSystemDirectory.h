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
		bool initialize( const ConstString& _path, bool _create ) override;
		bool existFile( const ConstString& _filename ) override;
		FileInputInterface* createInputFile() override;
		bool openInputFile( const ConstString& _filename, FileInputInterface* _file ) override;
		void closeInputFile( FileInputInterface* _inputFile ) override;
		
		FileOutputInterface* createOutputFile() override;
		bool openOutputFile( const ConstString& _filename, FileOutputInterface* _file ) override;
		void closeOutputFile( FileOutputInterface* _outputFile ) override;
		bool createDirectory( const ConstString& _path ) override;
		void removeDirectory( const ConstString& _path ) override;
		void removeFile( const ConstString& _filename ) override;

	private:
		void makeFullname_( const ConstString& _filename, String& _fullname );

	private:
		ConstString m_path;
		FileSystemInterface* m_interface;

		typedef Pool<BufferedFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;

		typedef Pool<SimpleFileOutput, PoolPlacementPolicyNone> TFileOutputPool;
		TFileOutputPool m_fileOutputPool;
	};
}	// namespace Menge
