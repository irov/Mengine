#include "MemoryCacheInput.h"

#include "Interface/MemoryInterface.h"

#include "MemoryService.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheInput::MemoryCacheInput()
        : m_memoryService( nullptr )
        , m_bufferId( 0 )
        , m_data( nullptr )
        , m_size( 0 )
        , m_carriage( nullptr )
        , m_end( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheInput::~MemoryCacheInput()
    {
        this->uncache_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheInput::uncache_()
    {
        if( m_bufferId != 0 )
        {
            m_memoryService->unlockBuffer( m_bufferId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheInput::setMemoryService( MemoryService * _memoryService )
    {
        m_memoryService = _memoryService;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryService * MemoryCacheInput::getMemoryService() const
    {
        return m_memoryService;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryCacheInput::cacheBuffer( size_t _size )
    {
        this->uncache_();

        void * memory;
        UniqueId bufferId = m_memoryService->lockBuffer( _size, &memory, MENGINE_DOCUMENT_FACTORABLE );

        if( bufferId == INVALID_UNIQUE_ID )
        {
            return nullptr;
        }

        m_bufferId = bufferId;

        m_data = reinterpret_cast<uint8_t *>(memory);
        m_size = _size;

        m_carriage = m_data;
        m_end = m_data + m_size;

        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryCacheInput::getBuffer() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryCacheInput::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryCacheInput::read( void * const _buf, size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        size_t size = _size;
        // Read over end of memory?
        if( m_carriage + size > m_end )
        {
            size = m_end - m_carriage;
        }

        if( size == 0 )
        {
            return 0;
        }

        Helper::memoryCopy( _buf, 0, m_carriage, 0, size );

        m_carriage += size;

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::seek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheInput::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        m_carriage = m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::rseek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + m_size - _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::skip( size_t _offset )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( m_carriage + _offset > m_end )
        {
            _offset = 0;
        }

        m_carriage += _offset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryCacheInput::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        return m_carriage == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryCacheInput::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        size_t distance = m_carriage - m_data;

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::memory( void ** const _memory, size_t * const _size )
    {
        *_memory = m_data;
        *_size = m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
