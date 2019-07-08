#include "MemoryInput.h"

#include "Kernel/MemoryAllocator.h"

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
    Pointer MemoryInput::newBuffer( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

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

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

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
    size_t MemoryInput::read( void * _buf, size_t _count )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::read" );

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
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::seek" );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::skip( size_t _pos )
    {
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::skip" );

        if( m_pos + _pos > m_end )
        {
            _pos = 0;
        }

        m_pos += _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::size() const
    {
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::size" );

        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::eof() const
    {
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::eof" );

        return m_pos == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::tell() const
    {
        STDEX_THREAD_GUARD_SCOPE( this, "MemoryInput::tell" );

        size_t distance = m_pos - m_data;

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::time( uint64_t & _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::memory( void ** _memory, size_t * _size )
    {
        *_memory = m_data;
        *_size = m_size;

        return true;
    }
}
