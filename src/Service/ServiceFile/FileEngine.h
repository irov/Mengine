#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Factory/FactoryManager.h"

#	include "Core/Pool.h"

#	include <string>
#	include <vector>
#	include <map>

namespace Menge
{
	class FileGroupInterface;

	class FileEngine
		: public FileServiceInterface
		, public FactoryManager
	{
	public:
		FileEngine();
		~FileEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;

	public:	// FileEngine Interface
		bool mountFileSystem( const ConstString& _fileSystemName, const FilePath& _path, const ConstString & _type, bool _create ) override;
		void unmountFileSystem( const ConstString& _fileSystemName ) override;

    public:
        bool hasFileSystem( const ConstString& _fileSystemName ) const override;
        FileGroupInterface * getFileSystem( const ConstString& _fileSystemName ) const override;
        
    public:
		bool existFile( const ConstString& _fileSystemName, const FilePath& _filename, FileGroupInterface ** _group ) const override;
		
		InputStreamInterface * openInputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
		OutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
        
    public:
        MappedFileInputStreamInterface * openMappedInputStream( const FilePath& _filename ) override;     
        
	public:
		bool createDirectory( const ConstString& _fileSystemName, const FilePath& _path );
		bool removeDirectory( const ConstString& _fileSystemName, const FilePath& _path );
		bool removeFile( const ConstString& _fileSystemName, const FilePath& _filename );

	private:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, FileGroupInterface*> TMapFileSystem;
		TMapFileSystem m_fileSystemMap;

		FileSystemInterface * m_fileSystem;
	};
}
