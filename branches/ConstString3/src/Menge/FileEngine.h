#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Core/Holder.h"

#	include <string>
#	include <vector>
#	include <map>

#	include "FactoryManager.h"

#	include "FileInput.h"
#	include "Core/Pool.h"

namespace Menge
{
	class FileGroupInterface;

	class FileEngine
		: public Holder<FileEngine>
		, public FactoryManager
		, public FileServiceInterface
	{
	public:
		FileEngine();
		~FileEngine();

	public:
		bool initialize();

	public:	// FileEngine Interface
		bool mountFileSystem( const ConstString& _fileSystemName, const WString& _path, const ConstString & _type, bool _create ) override;
		void unmountFileSystem( const ConstString& _fileSystemName ) override;

		bool getFullPath( const ConstString& _fileSystemName, const WString & _path, WString & _fullPath ) const /*override*/;
		bool getFileSystemPath( const ConstString& _fileSystemName, WString & _fileSystemPath ) const /*override*/;

		bool existFile( const ConstString& _fileSystemName, const WString& _filename ) const override;
		
		//FileInputStreamInterface * createInputFile( const ConstString& _fileSystemName ) override;
		FileInputStreamInterface * openInputFile( const ConstString& _fileSystemName, const WString& _filename ) override;

		//FileOutputStreamInterface * createOutputFile( const ConstString& _fileSystemName ) override;
		FileOutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const WString& _filename ) override;

	public:
		FileGroupInterface * getFileSystem( const ConstString& _fileSystemName ) const;

	public:
		bool createDirectory( const ConstString& _fileSystemName, const WString& _path );
		void removeDirectory( const ConstString& _fileSystemName, const WString& _path );
		void removeFile( const ConstString& _fileSystemName, const WString& _filename );

		FileSystemInterface* getFileSystemInterface() const;

	private:
		typedef std::map<ConstString, FileGroupInterface*> TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;

		//FileSystemMemoryMapped * m_fileSystemMemoryMapped;
	};
}
