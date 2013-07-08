#	pragma once

#	include "MarmaladeInputStream.h"

#   include "Utils/Core/MemoryInput.h"

#   include "Utils/Factory/FactoryPool.h"

namespace Menge
{
	class MarmaladeMappedInputStream
		: public MappedFileInputStreamInterface
	{
	public:
		MarmaladeMappedInputStream();
		~MarmaladeMappedInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        InputStreamInterfacePtr createInputMemory() override;
        void openInputMemory( const InputStreamInterfacePtr & _stream, const FilePath & _filename, size_t _offset, size_t _size ) override;

    public:
        bool open( const FilePath & _folder, const FilePath& _filename ) override;

    public:
        size_t read( void* _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;		

    public:
        bool time( uint64 & _time ) const override;

    public:
        bool _destroy() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        typedef FactoryPool<MemoryInput, 16> TFactoryMemoryInput;
        TFactoryMemoryInput m_factoryMemoryInput;               

        MarmaladeInputStream m_inputStream;
	};
}	// namespace Menge
