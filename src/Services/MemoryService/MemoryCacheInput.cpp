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
        , m_pos( nullptr )
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

        m_pos = m_data;
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

        size_t cnt = _size;
        // Read over end of memory?
        if( m_pos + cnt > m_end )
        {
            cnt = m_end - m_pos;
        }

        if( cnt == 0 )
        {
            return 0;
        }

        Helper::memoryCopy( _buf, 0, m_pos, 0, cnt );

        m_pos += cnt;

        return cnt;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryCacheInput::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        m_pos = m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + m_size - _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryCacheInput::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        if( m_pos + _size > m_end )
        {
            _size = 0;
        }

        m_pos += _size;

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

        return m_pos == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryCacheInput::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryCacheInput, this );

        size_t distance = m_pos - m_data;

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
