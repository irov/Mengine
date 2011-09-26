#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Core/Holder.h"

#	include <string>
#	include <vector>
#	include <map>

#	include "FactoryManager.h"

#	include "FileSystemMemoryMapped.h"
#	include "FileInput.h"
#	include "FileOutput.h"
#	include "Core/Pool.h"

namespace Menge
{
	class FileSystem;

	class FileEngine
		: public Holder<FileEngine>
		, public FactoryManager
	{
	public:
		FileEngine();
		~FileEngine();

	public:
		bool initialize();

	public:	// FileEngine Interface
		bool mountFileSystem( const ConstString& _fileSystemName, const String& _path, const ConstString & _type, bool _create );
		void unmountFileSystem( const ConstString& _fileSystemName );

		bool getFullPath( const ConstString& _fileSystemName, const ConstString & _path, String & _fullPath ) const;
		bool getFileSystemPath( const ConstString& _fileSystemName, String & _fileSystemPath ) const;

		bool existFile( const ConstString& _fileSystemName, const String& _filename );

		FileInputInterface * createInputFile( const ConstString& _fileSystemName );
		FileInputInterface * openInputFile( const ConstString& _fileSystemName, const String& _filename );

		FileInputInterface * openMappedFile( const String& _filename );

		FileOutputInterface * createOutputFile( const ConstString& _fileSystemName );
		FileOutputInterface * openOutputFile( const ConstString& _fileSystemName, const String& _filename );

		void setBaseDir( const String& _baseDir );
		const String& getBaseDir() const;

		bool createDirectory( const ConstString& _fileSystemName, const String& _path );
		void removeDirectory( const ConstString& _fileSystemName, const String& _path );
		void removeFile( const ConstString& _fileSystemName, const String& _filename );

		FileSystemInterface* getFileSystemInterface();

		static bool s_isAbsolutePath( const String& _path );

	private:
		typedef std::map<ConstString, FileSystem*> TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;
		
		String m_baseDir;

		FileSystemMemoryMapped * m_fileSystemMemoryMapped;
	};
}
