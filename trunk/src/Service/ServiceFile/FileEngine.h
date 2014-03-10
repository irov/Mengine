#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Factory/FactoryManager.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
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
		bool mountFileGroup( const ConstString& _fileGroupName, const FilePath& _folder, const FilePath& _path, const ConstString & _type, bool _create ) override;
		void unmountFileGroup( const ConstString& _fileGroupName ) override;

    public:
        bool hasFileGroup( const ConstString& _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const override;
        FileGroupInterfacePtr getFileGroup( const ConstString& _fileGroupName ) const override;
        
    public:
		bool existFile( const ConstString& _fileGroupName, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const override;

	public:
		InputStreamInterfacePtr openInputFile( const ConstString& _fileGroupName, const FilePath& _fileName ) override;
		OutputStreamInterfacePtr openOutputFile( const ConstString& _fileGroupName, const FilePath& _fileName ) override;
        
    public:
        MappedFileInterfacePtr createMappedFile( const FilePath & _fileGroupName, const FilePath& _fileName ) override;
		MappedFileInterfacePtr createSharedFile( const FilePath & _fileGroupName, const FilePath& _fileName ) override;
        
	public:
        bool existDirectory( const ConstString& _fileGroupName, const FilePath& _path ) const override;
		bool createDirectory( const ConstString& _fileGroupName, const FilePath& _path ) override;
		bool removeDirectory( const ConstString& _fileGroupName, const FilePath& _path ) override;
		bool removeFile( const ConstString& _fileGroupName, const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        FactoryManager m_factoryFileGroup;

		typedef stdex::binary_vector<ConstString, FileGroupInterfacePtr> TMapFileSystem;
		TMapFileSystem m_fileSystemMap;
	};
}
