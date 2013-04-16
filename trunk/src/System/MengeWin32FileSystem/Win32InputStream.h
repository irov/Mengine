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
		Win32InputStream( ServiceProviderInterface * _serviceProvider );
		~Win32InputStream();

	public:
		bool open( const FilePath& _filename ) override;
		
	public:
		size_t read( void* _buf, size_t _count ) override;
		void seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( time_t & _time ) const override;

    public:
        void destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		HANDLE m_hFile;
		DWORD m_size;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;

        char m_buff[FILE_BUFFER_SIZE];
	};
}	// namespace Menge
