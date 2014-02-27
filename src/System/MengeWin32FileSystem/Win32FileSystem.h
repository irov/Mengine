#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#	include "Win32InputStream.h"
#	include "Win32OutputStream.h"
#	include "Win32MappedInputStream.h"

#   include "Factory/FactoryStore.h"

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
		FileInputStreamInterfacePtr createInputStream() override;
		FileOutputStreamInterfacePtr createOutputStream() override;
		
    public:
        MappedFileInputStreamInterfacePtr createMappedInputStream() override;

    public:
		bool existFile( const FilePath & _folder, const FilePath& _dir, const char * _filename, size_t _filenamelen  ) const override;
		
	public:
		bool deleteFile( const FilePath & _folder, const FilePath& _filename ) override;

        bool existFolder( const FilePath & _folder, const FilePath& _filename  ) const override;
		bool createFolder( const FilePath & _folder, const FilePath& _path ) override;
		bool deleteFolder( const FilePath & _folder, const FilePath& _path ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<Win32InputStream, 8> TFactoryFileInputStream;
        TFactoryFileInputStream m_factoryInputStream;

        typedef FactoryPoolStore<Win32OutputStream, 4> TFactoryFileOutputStream;
        TFactoryFileOutputStream m_factoryOutputStream;

        typedef FactoryPoolStore<Win32MappedInputStream, 4> TFactoryMappedInputStream;
        TFactoryMappedInputStream m_factoryMappedInputStream;
    };
}
