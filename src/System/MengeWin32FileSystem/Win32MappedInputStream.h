#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

namespace Menge
{
	class Win32MappedInputStream
		: public MappedFileInputStreamInterface
	{
	public:
		Win32MappedInputStream( ServiceProviderInterface * _serviceProvider );
		~Win32MappedInputStream();

    public:
        InputStreamInterface * createInputMemory() override;
        void openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size ) override;

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
		HANDLE m_hMapping;

        void * m_memory;

        unsigned char* m_data;
        unsigned char* m_pos;
        unsigned char* m_end;
        size_t m_size;		
	};
}	// namespace Menge
