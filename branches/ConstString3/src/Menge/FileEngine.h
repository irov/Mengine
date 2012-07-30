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
	class FileGroup;

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
		
		FileInputStreamInterface * createInputFile( const ConstString& _fileSystemName ) override;
		FileInputStreamInterface * openInputFile( const ConstString& _fileSystemName, const WString& _filename ) override;

		FileOutputStreamInterface * createOutputFile( const ConstString& _fileSystemName ) override;
		FileOutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const WString& _filename ) override;

	public:
		FileGroup * getFileSystem( const ConstString& _fileSystemName ) const;

	public:
		bool createDirectory( const ConstString& _fileSystemName, const WString& _path );
		void removeDirectory( const ConstString& _fileSystemName, const WString& _path );
		void removeFile( const ConstString& _fileSystemName, const WString& _filename );

		FileSystemInterface* getFileSystemInterface() const;

	private:
		bool createDirectoryPathFileSystem_( FileGroup * _fs, const WString& _path ) const;

	private:
		typedef std::map<ConstString, FileGroup*> TFileSystemMap;
		TFileSystemMap m_fileSystemMap;

		FileSystemInterface * m_interface;

		//FileSystemMemoryMapped * m_fileSystemMemoryMapped;
	};
}
