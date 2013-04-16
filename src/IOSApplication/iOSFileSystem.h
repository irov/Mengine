#   pragma once

#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ServiceInterface.h"

#   include <map>

namespace Menge
{

    class iOSFileInput 
        : public FileInputStreamInterface
    {
    public:
        iOSFileInput( ServiceProviderInterface * _serviceProvider );

    public:
        bool open( const FilePath & _filename ) override;

    public:
        int read( void * _buf, int _count ) override;
        void seek( int _pos ) override;
        int	tell( void ) const override;
        int size( void ) const override;

    public:
        bool time( time_t & _time ) const override;

    public:
        void destroy() override;

    public:
        ServiceProviderInterface * m_serviceProvider;

        FILE * stream;        
    };

    class iOSMappedFileInput 
        : public MappedFileInputStreamInterface
    {
    public:
        iOSMappedFileInput( ServiceProviderInterface * _serviceProvider );

    public:
        InputStreamInterface * createInputMemory() override;
        void openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size ) override;

    public:
        bool open( const FilePath & _filename ) override;

    public:
        int read( void * _buf, int _count ) override;
        void seek( int _pos ) override;
        int	tell( void ) const override;
        int size( void ) const override;

    public:
        bool time( time_t & _time ) const override;

    public:
        void destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        int m_id;

        void * m_memory;

        unsigned char * m_data;
        unsigned char * m_pos;
        unsigned char * m_end;

        int m_size;		
    };

    class iOSFileOutput 
        : public FileOutputStreamInterface
    {
    public:
        iOSFileOutput( ServiceProviderInterface * _serviceProvider );

    public:
        bool open( const FilePath & _filename ) override;
        
    public:
        void write( const void* _data, int _count ) override;
        void flush( void ) override;

    public:
        void destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        FILE * stream;        
    };

    class iOSFileSystem 
        : public FileSystemInterface
    {
    public:
        iOSFileSystem();
        ~iOSFileSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool existFile( const FilePath & _filename ) const override;

    public:
        FileInputStreamInterface* createInputStream() override;
        FileOutputStreamInterface* createOutputStream() override;

    public:
        MappedFileInputStreamInterface* createMappedInputStream() override;

    public:
        bool deleteFile( const FilePath& _filename ) override;
        bool createFolder( const FilePath& _path ) override;
        bool deleteFolder( const FilePath& _path ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef std::map<void *, FILE *> TMappedFiles;
        TMappedFiles mappedFiles;
    };

}
