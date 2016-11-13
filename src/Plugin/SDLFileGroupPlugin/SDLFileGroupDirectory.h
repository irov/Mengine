#	pragma once

#   include "Interface/FileSystemInterface.h"

#   include "Factory/FactoryStore.h"

#	include "SDLFileInputStream.h"
#	include "SDLFileOutputStream.h"
#	include "SDLFileMapped.h"

namespace Menge
{
    class SDLFileGroupDirectory
        : public FileGroupInterface
    {
    public:
        SDLFileGroupDirectory();
        ~SDLFileGroupDirectory();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const FilePath & _path ) override;
        void finalize() override;

    public:
        bool isPacked() const override;

    public:
        const FilePath & getPath() const override;

    public:
        bool existFile( const FilePath & _fileName ) const override;
        bool removeFile( const FilePath& _fileName ) override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
        bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
        
    public:
        OutputStreamInterfacePtr createOutputFile() override;        
        bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

    public:
        FileMappedInterfacePtr createMappedFile() override;
        bool openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream ) override;

    public:
        bool existDirectory( const FilePath& _path ) const override;
        bool createDirectory( const FilePath& _path ) override;
        bool removeDirectory( const FilePath& _path ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_path;

        typedef FactoryPoolStore<SDLFileInputStream, 8> TFactoryFileInputStream;
        TFactoryFileInputStream m_factoryInputStream;

        typedef FactoryPoolStore<SDLFileOutputStream, 4> TFactoryFileOutputStream;
        TFactoryFileOutputStream m_factoryOutputStream;

        //typedef FactoryPoolStore<SDLFileMapped, 4> TFactoryWin32MappedFile;
        //TFactoryWin32MappedFile m_factoryWin32MappedFile;
    };
}	// namespace Menge
