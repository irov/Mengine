#include "Win32FileMappedInputStream.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileMappedInputStream::Win32FileMappedInputStream()
        : m_memory( NULL )
        , m_size( 0 )
        , m_pos( nullptr )
        , m_end( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileMappedInputStream::~Win32FileMappedInputStream()
    {
        this->unmap();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::map( HANDLE _hMapping, size_t _offset, size_t _size )
    {
        DWORD64 offset = (DWORD64)_offset;
        DWORD offsetHigh = (offset >> 32) & 0xFFFFFFFFlu;
        DWORD offsetLow = (offset >> 0) & 0xFFFFFFFFlu;

        LPVOID memory = ::MapViewOfFile( _hMapping, FILE_MAP_READ, offsetHigh, offsetLow, (DWORD)_size );

        if( memory == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid map view of file [error: %lu]"
                , error
            );

            return false;
        }

        m_memory = memory;
        m_size = _size;

        m_pos = (uint8_t *)m_memory;
        m_end = (uint8_t *)m_memory + m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileMappedInputStream::unmap()
    {
        if( ::UnmapViewOfFile( m_memory ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid UnmapViewOfFile %p [error: %lu]"
                , m_memory
                , error
            );
        }

        m_memory = NULL;
        m_size = 0;

        m_pos = nullptr;
        m_end = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileMappedInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::read" );

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
    bool Win32FileMappedInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::seek" );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = (uint8_t *)m_memory + _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::rseek" );

        if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = (uint8_t *)m_memory + m_size - _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::skip( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::skip" );

        if( m_pos + _pos > m_end )
        {
            _pos = 0;
        }

        m_pos += _pos;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileMappedInputStream::size() const
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::size" );

        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::eof" );

        return m_pos == m_end;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileMappedInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::tell" );

        size_t distance = m_pos - (uint8_t *)m_memory;

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedInputStream::memory( void ** const _memory, size_t * const _size )
    {
        *_memory = (uint8_t *)m_memory;
        *_size = m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
