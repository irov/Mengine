#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Factory/FactoryManager.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class FileGroupInterface;

	class FileEngine
		: public FileServiceInterface
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
		bool mountFileGroup( const ConstString& _fileSystemName, const FilePath& _folder, const FilePath& _path, const ConstString & _type, bool _create ) override;
		void unmountFileGroup( const ConstString& _fileSystemName ) override;

    public:
        bool hasFileGroup( const ConstString& _fileSystemName, FileGroupInterface ** _fileGroup ) const override;
        FileGroupInterface * getFileGroup( const ConstString& _fileSystemName ) const override;
        
    public:
		bool existFile( const ConstString& _fileSystemName, const FilePath& _dir, const char * _filename, size_t _filenamelen, FileGroupInterface ** _group ) const override;

	public:
		InputStreamInterfacePtr openInputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
		OutputStreamInterfacePtr openOutputFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;
        
    public:
        MappedFileInterfacePtr createMappedFile( const FilePath & _foldername, const FilePath& _filename ) override;
		MappedFileInterfacePtr createSharedFile( const FilePath & _foldername, const FilePath& _filename ) override;
        
	public:
        bool existDirectory( const ConstString& _fileSystemName, const FilePath& _path ) override;
		bool createDirectory( const ConstString& _fileSystemName, const FilePath& _path ) override;
		bool removeDirectory( const ConstString& _fileSystemName, const FilePath& _path ) override;
		bool removeFile( const ConstString& _fileSystemName, const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        FactoryManager m_factoryFileGroup;

		typedef stdex::binary_vector<ConstString, FileGroupInterface *> TMapFileSystem;
		TMapFileSystem m_fileSystemMap;
	};
}
