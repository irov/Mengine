#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

namespace Menge
{
    class ServiceProviderInterface;

    const size_t FILE_BUFFER_SIZE = 8192; //8kb

	class Win32InputStream
		: public FileInputStreamInterface
	{
	public:
		Win32InputStream();
		~Win32InputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen ) override;
		
	public:
		size_t read( void* _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( uint64_t & _time ) const override;

    protected:
        void close_();

    protected:
        ServiceProviderInterface * m_serviceProvider;

		HANDLE m_hFile;
		DWORD m_size;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;

        char m_buff[FILE_BUFFER_SIZE];
	};
}	// namespace Menge
