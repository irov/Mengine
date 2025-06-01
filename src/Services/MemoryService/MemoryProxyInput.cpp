#include "MemoryProxyInput.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInput::MemoryProxyInput()
        : m_data( nullptr )
        , m_size( 0 )
        , m_carriage( nullptr )
        , m_end( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInput::~MemoryProxyInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryProxyInput::setBuffer( void * _memory, size_t _offset, size_t _size )
    {
        m_data = static_cast<uint8_t *>(_memory) + _offset;
        m_size = _size;

        m_carriage = m_data;
        m_end = m_data + m_size;

        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryProxyInput::getBuffer() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryProxyInput::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryProxyInput::read( void * _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        size_t cnt = _count;
        // Read over end of memory?
        if( m_carriage + cnt > m_end )
        {
            cnt = m_end - m_carriage;
        }

        if( cnt == 0 )
        {
            return 0;
        }

        Helper::memoryCopy( _buf, 0, m_carriage, 0, cnt );

        m_carriage += cnt;

        return cnt;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::seek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryProxyInput::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        m_carriage = m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::rseek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + m_size - _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::skip( size_t _offset )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        if( m_carriage + _offset > m_end )
        {
            _offset = 0;
        }

        m_carriage += _offset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryProxyInput::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        return m_carriage == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryProxyInput::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryProxyInput, this );

        size_t distance = m_carriage - m_data;

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::time( uint64_t * _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxyInput::memory( void ** const _memory, size_t * const _size )
    {
        *_memory = m_data;
        *_size = m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
