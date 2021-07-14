#include "MemoryCacheBuffer.h"

#include "Interface/MemoryInterface.h"

#include "MemoryService.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheBuffer::MemoryCacheBuffer()
        : m_memoryManager( nullptr )
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
    void MemoryCacheBuffer::setMemoryManager( MemoryService * _memoryManager )
    {
        m_memoryManager = _memoryManager;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::uncache_()
    {
        if( m_bufferId != 0 )
        {
            m_memoryManager->unlockBuffer( m_bufferId );
            m_bufferId = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheBuffer::setBuffer( const void * _ptr, size_t _size )
    {
        void * buffer = this->newBuffer( _size );

        stdex::memorycopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryCacheBuffer::newBuffer( size_t _size )
    {
        this->uncache_();

        void * memory;
        UniqueId bufferId = m_memoryManager->lockBuffer( _size, &memory, MENGINE_DOCUMENT_FACTORABLE );

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
