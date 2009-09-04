/*
 *	FileSystemDirectory.h
 *
 *	Created by _Berserk_ on 27.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileSystem.h"
#	include "Pool.h"

namespace Menge
{
	class FileSystemInterface;
	class BufferedFileInput;
	class FileOutput;

	class FileSystemDirectory
		: public FileSystem
	{
	public:
		FileSystemDirectory();
		~FileSystemDirectory();

		bool initialize( const String& _path, bool _create ) override;
		bool existFile( const String& _filename ) override;
		FileInputInterface* openInputFile( const String& _filename ) override;
		void closeInputFile( FileInputInterface* _inputFile ) override;
		
		FileOutputInterface* openOutputFile( const String& _filename ) override;
		void closeOutputFile( FileOutputInterface* _outputFile ) override;
		bool createDirectory( const String& _path ) override;
		void removeDirectory( const String& _path ) override;
		void removeFile( const String& _filename ) override;

	private:
		void makeFullname_( const String& _filename, String* _fullname );

	private:
		String m_path;
		FileSystemInterface* m_interface;

		typedef Pool<BufferedFileInput, PoolPlacementPolicyNone> TFileInputPool;
		TFileInputPool m_fileInputPool;

		typedef Pool<FileOutput, PoolPlacementPolicyNone> TFileOutputPool;
		TFileOutputPool m_fileOutputPool;
	};
}	// namespace Menge
