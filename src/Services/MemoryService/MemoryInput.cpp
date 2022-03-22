#include "MemoryInput.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/ThreadGuardScope.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryInput::MemoryInput()
        : m_data( nullptr )
        , m_size( 0 )
        , m_pos( nullptr )
        , m_end( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInput::~MemoryInput()
    {
        Helper::freeArrayT( m_data );
        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryInput::newBuffer( size_t _size )
    {
        uint8_t * memory = Helper::reallocateArrayT<uint8_t>( m_data, _size );

        if( memory == nullptr )
        {
            m_data = nullptr;
            m_size = 0;
            m_pos = 0;
            m_end = 0;

            return nullptr;
        }

        m_data = memory;
        m_size = _size;
        m_pos = m_data;
        m_end = m_data + m_size;

        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryInput::getBuffer() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::read" );

        size_t cnt = _count;

        // Read over end of memory?
        if( m_pos + cnt > m_end )
        {
            cnt = m_end - m_pos;
        }

        if( cnt == 0 )
        {
            return 0;
        }

        stdex::memorycopy( _buf, 0, m_pos, cnt );

        m_pos += cnt;

        return cnt;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::seek" );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryInput::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::rewind" );

        m_pos = m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::rseek" );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + m_size - _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::skip" );

        if( m_pos + _size > m_end )
        {
            _size = 0;
        }

        m_pos += _size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::size() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::size" );

        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::eof" );

        return m_pos == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this, "MemoryInput::tell" );

        size_t distance = m_pos - m_data;

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::memory( void ** const _memory, size_t * const _size )
    {
        *_memory = m_data;
        *_size = m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
