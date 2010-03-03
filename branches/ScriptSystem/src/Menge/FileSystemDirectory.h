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
		bool initialize( const String& _path, bool _create ) override;
		bool existFile( const String& _filename ) override;
		FileInput* createInputFile() override;
		bool openInputFile( const String& _filename, FileInput* _file ) override;
		void closeInputFile( FileInput* _inputFile ) override;
		
		FileOutput* createOutputFile() override;
		bool openOutputFile( const String& _filename, FileOutput* _file ) override;
		void closeOutputFile( FileOutput* _outputFile ) override;
		bool createDirectory( const String& _path ) override;
		void removeDirectory( const String& _path ) override;
		void removeFile( const String& _filename ) override;

	private:
		void makeFullname_( const String& _filename, String& _fullname );

	private:
		String m_path;
		FileSystemInterface* m_interface;

		typedef Pool<BufferedFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;

		typedef Pool<SimpleFileOutput, PoolPlacementPolicyNone> TFileOutputPool;
		TFileOutputPool m_fileOutputPool;
	};
}	// namespace Menge
