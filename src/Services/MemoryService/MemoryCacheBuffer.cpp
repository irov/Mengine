#include "MemoryCacheBuffer.h"

#include "Interface/MemoryInterface.h"

#include "MemoryService.h"

#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheBuffer::MemoryCacheBuffer()
        : m_memoryService( nullptr )
        , m_bufferId( 0 )
        , m_data( nullptr )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheBuffer::~MemoryCacheBuffer()
    {
        this->uncache_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::setMemoryService( MemoryService * _memoryService )
    {
        m_memoryService = _memoryService;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryService * MemoryCacheBuffer::setMemoryService() const
    {
        return m_memoryService;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::uncache_()
    {
        if( m_bufferId != 0 )
        {
            m_memoryService->unlockBuffer( m_bufferId );
            m_bufferId = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::setBuffer( const void * _ptr, size_t _size )
    {
        void * buffer = this->newBuffer( _size );

        Helper::memoryCopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryCacheBuffer::newBuffer( size_t _size )
    {
        this->uncache_();

        void * memory;
        UniqueId bufferId = m_memoryService->lockBuffer( _size, &memory, MENGINE_DOCUMENT_FACTORABLE );

        if( bufferId == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        m_bufferId = bufferId;

        m_data = memory;
        m_size = _size;

        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::clearBuffer()
    {
        this->uncache_();

        m_data = nullptr;
        m_size = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
