#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Holder.h"

#	include <string>
#	include <vector>
#	include <map>

#	include "FactoryManager.h"

#	include "FileInterface.h"

namespace Menge
{
	typedef std::vector<unsigned char> Blobject;

	class FileSystem;

	class FileEngine
	{
	public:
		FileEngine( FileSystemInterface * _interface );
		~FileEngine();

	public:	// FileEngine Interface
		bool mountFileSystem( const String& _fileSystemName, const String& _path, bool _create );
		void unmountFileSystem( const String& _fileSystemName, bool _delete );

		FileInputInterface* openFileInput( const String& _fileSystemName, const String& _filename );
		void closeFileInput( FileInputInterface* _file );

		FileOutputInterface* openFileOutput( const String& _fileSystemName, const String& _filename );
		void closeFileOutput( FileOutputInterface* _outStream );

		void setBaseDir( const String& _baseDir );
		const String& getBaseDir() const;

	private:
		FactoryManager m_fileSystemFactoryMgr;

		typedef std::map< String, FileSystem* > TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;

		String m_baseDir;
	};
}
