#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include <map>


namespace Menge
{
    class WindowsLayerInterface;

    class LogServiceInterface;
    class ServiceProviderInterface;

	class Win32FileSystem
		: public FileSystemInterface
	{
	public:
		Win32FileSystem();
		~Win32FileSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

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
