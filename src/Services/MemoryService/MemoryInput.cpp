#include "MemoryInput.h"

#include "Kernel/MemoryAllocator.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryInput::MemoryInput()
        : m_data( nullptr )
        , m_size( 0 )
        , m_carriage( nullptr )
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
            m_carriage = 0;
            m_end = 0;

            return nullptr;
        }

        m_data = memory;
        m_size = _size;
        m_carriage = m_data;
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
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

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
    bool MemoryInput::seek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryInput::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        m_carriage = m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::rseek( size_t _carriage )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        if( _carriage > m_size )
        {
            _carriage = m_size;
        }

        m_carriage = m_data + m_size - _carriage;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::skip( size_t _offset )
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        if( m_carriage + _offset > m_end )
        {
            _offset = 0;
        }

        m_carriage += _offset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::size() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryInput::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        return m_carriage == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryInput::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( MemoryInput, this );

        size_t distance = m_carriage - m_data;

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
