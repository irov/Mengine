#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "MarmaladeInputStream.h"
#	include "MarmaladeOutputStream.h"
#	include "MarmaladeMappedInputStream.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
    class WindowsLayerInterface;

    class LogServiceInterface;
    class ServiceProviderInterface;

	class MarmaladeFileSystem
		: public FileSystemInterface
	{
	public:
		MarmaladeFileSystem();
		~MarmaladeFileSystem();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) ;
		ServiceProviderInterface * getServiceProvider() const;

	public:
		FileInputStreamInterfacePtr createInputStream() override;
		FileOutputStreamInterfacePtr createOutputStream() override;
		
    public:
        MappedFileInterfacePtr createMappedFile() override;
		MappedFileInterfacePtr createSharedFile() override;

    public:
		bool existFile( const FilePath & _folder, const FilePath & _fileName ) const override;
		bool deleteFile( const FilePath & _folder, const FilePath& _filename ) override;

    public:
        bool existFolder( const FilePath & _folder, const FilePath & _filename ) const override;
		bool createFolder( const FilePath & _folder, const FilePath& _filename ) override;
		bool deleteFolder( const FilePath & _folder, const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<MarmaladeInputStream, 8> TFactoryFileInputStream;
        TFactoryFileInputStream m_factoryInputStream;

        typedef FactoryPoolStore<MarmaladeOutputStream, 4> TFactoryFileOutputStream;
        TFactoryFileOutputStream m_factoryOutputStream;

        typedef FactoryPoolStore<MarmaladeMappedInputStream, 4> TFactoryMappedInputStream;
        TFactoryMappedInputStream m_factoryMappedInputStream;
	};
}
