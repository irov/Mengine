#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#   include "Core/MemoryProxyInput.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
	class Win32MappedInputStream
		: public MappedFileInputStreamInterface
	{
	public:
		Win32MappedInputStream();
		~Win32MappedInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        InputStreamInterfacePtr createInputMemory() override;
        void openInputMemory( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;

	public:
		bool open( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen ) override;

	public:
		size_t read( void* _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( uint64_t & _time ) const override;
        
	private:
        ServiceProviderInterface * m_serviceProvider;
        
		HANDLE m_hFile;
		HANDLE m_hMapping;

        void * m_memory;

        unsigned char* m_data;
        unsigned char* m_pos;
        unsigned char* m_end;
        size_t m_size;

        typedef FactoryPoolStore<MemoryProxyInput, 16> TFactoryMemoryProxyInput;
        TFactoryMemoryProxyInput m_factoryMemoryProxyInput;
	};
}	// namespace Menge
