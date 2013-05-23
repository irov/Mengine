#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#   include "Core/MemoryProxyInput.h"

#   include "Utils/Factory/FactoryPool.h"

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
        void openInputMemory( const InputStreamInterfacePtr & _stream, const FilePath & _filename, size_t _offset, size_t _size ) override;

	public:
		bool open( const FilePath & _folder, const FilePath& _filename ) override;		

	public:
		size_t read( void* _buf, size_t _count ) override;
		void seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( time_t & _time ) const override;

    public:
        bool _destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;
        
		HANDLE m_hFile;
		HANDLE m_hMapping;

        void * m_memory;

        unsigned char* m_data;
        unsigned char* m_pos;
        unsigned char* m_end;
        size_t m_size;

        typedef FactoryPool<MemoryProxyInput, 16> TFactoryMemoryProxyInput;
        TFactoryMemoryProxyInput m_factoryMemoryProxyInput;
	};
}	// namespace Menge
