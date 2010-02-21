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

		bool initialize();

	public:	// FileEngine Interface
		bool mountFileSystem( const String& _fileSystemName, const String& _path, bool _create );
		void unmountFileSystem( const String& _fileSystemName );

		bool existFile( const String& _fileSystemName, const String& _filename );

		FileInput* createFileInput( const String& _fileSystemName );
		FileInput* openFileInput( const String& _fileSystemName, const String& _filename );
		void closeFileInput( FileInput* _file );

		FileInput* openMappedFile( const String& _filename );
		void closeMappedFile( FileInput* _file );

		FileOutput* openFileOutput( const String& _fileSystemName, const String& _filename );
		void closeFileOutput( FileOutput* _outStream );

		void setBaseDir( const String& _baseDir );
		const String& getBaseDir() const;

		bool createDirectory( const String& _fileSystemName, const String& _path );
		void removeDirectory( const String& _fileSystemName, const String& _path );
		void removeFile( const String& _fileSystemName, const String& _filename );

		FileSystemInterface* getFileSystemInterface();

		static bool s_isAbsolutePath( const String& _path );

	private:
		typedef std::map<String, FileSystem*> TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;

		String m_baseDir;

		FileSystemMemoryMapped * m_fileSystemMemoryMapped;
	};
}
