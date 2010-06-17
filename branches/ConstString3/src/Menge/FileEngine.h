#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Core/Holder.h"

#	include <string>
#	include <vector>
#	include <map>

#	include "Factory/FactoryManager.h"

#	include "FileSystemMemoryMapped.h"
#	include "FileInput.h"
#	include "FileOutput.h"
#	include "Core/Pool.h"

namespace Menge
{
	typedef std::vector<unsigned char> Blobject;

	class FileSystem;

	class FileEngine
		: public FactoryManager
		, public Holder<FileEngine>
	{
	public:
		FileEngine();
		~FileEngine();

	public:
		bool initialize();

	public:	// FileEngine Interface
		bool mountFileSystem( const ConstString& _fileSystemName, const ConstString& _path, bool _create );
		void unmountFileSystem( const ConstString& _fileSystemName );

		bool existFile( const ConstString& _fileSystemName, const ConstString& _filename );

		FileInputInterface * createFileInput( const ConstString& _fileSystemName );
		FileInputInterface * openFileInput( const ConstString& _fileSystemName, const ConstString& _filename );
		void closeFileInput( FileInputInterface * _file );

		FileInputInterface * openMappedFile( const ConstString& _filename );
		void closeMappedFile( FileInputInterface * _file );

		FileOutputInterface * createFileOutput( const ConstString& _fileSystemName );
		FileOutputInterface * openFileOutput( const ConstString& _fileSystemName, const ConstString& _filename );
		void closeFileOutput( FileOutputInterface* _outStream );

		void setBaseDir( const ConstString& _baseDir );
		const ConstString& getBaseDir() const;

		bool createDirectory( const ConstString& _fileSystemName, const ConstString& _path );
		void removeDirectory( const ConstString& _fileSystemName, const ConstString& _path );
		void removeFile( const ConstString& _fileSystemName, const ConstString& _filename );

		FileSystemInterface* getFileSystemInterface();

		static bool s_isAbsolutePath( const String& _path );

	private:
		typedef std::map<ConstString, FileSystem*> TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;
		
		ConstString m_baseDir;

		FileSystemMemoryMapped * m_fileSystemMemoryMapped;
	};
}
