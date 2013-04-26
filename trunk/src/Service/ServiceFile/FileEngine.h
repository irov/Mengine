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
		bool mountFileGroup( const ConstString& _fileSystemName, const FilePath& _path, const ConstString & _type, bool _create ) override;
		void unmountFileGroup( const ConstString& _fileSystemName ) override;

    public:
        bool hasFileGroup( const ConstString& _fileSystemName ) const override;
        FileGroupInterface * getFileGroup( const ConstString& _fileSystemName ) const override;
        
    public:
		bool existFile( const ConstString& _fileSystemName, const FilePath& _filename, FileGroupInterface ** _group ) const override;
		
		InputStreamInterfacePtr openInputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
		OutputStreamInterfacePtr openOutputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
        
    public:
        MappedFileInputStreamInterfacePtr openMappedInputStream( const FilePath& _filename ) override;     
        
	public:
		bool createDirectory( const ConstString& _fileSystemName, const FilePath& _path );
		bool removeDirectory( const ConstString& _fileSystemName, const FilePath& _path );
		bool removeFile( const ConstString& _fileSystemName, const FilePath& _filename );

	private:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, FileGroupInterface*> TMapFileSystem;
		TMapFileSystem m_fileSystemMap;
	};
}
