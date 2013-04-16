#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include <s3eFile.h>

#	include <map>

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
        bool initialize( ServiceProviderInterface * _serviceProvider ) override;

	public:
		FileInputStreamInterface* createInputStream() override;
		FileOutputStreamInterface* createOutputStream() override;
		
    public:
        MappedFileInputStreamInterface * createMappedInputStream() override;

    public:
		bool existFile( const FilePath& _filename  ) const override;
		bool deleteFile( const FilePath& _filename ) override;

		bool createFolder( const FilePath& _path ) override;
		bool deleteFolder( const FilePath& _path ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;
	};
}
