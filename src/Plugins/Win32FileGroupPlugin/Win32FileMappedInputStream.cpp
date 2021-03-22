#include "Win32FileMappedInputStream.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileMappedInputStream::Win32FileMappedInputStream()
        : m_memoryGranularity( NULL )
        , m_size( 0 )
        , m_base( nullptr )
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
    bool Win32FileMappedInputStream::mapViewOfFile( HANDLE _hMapping, DWORD _dwAllocationGranularity, size_t _offset, size_t _size )
    {
#if !defined(MENGINE_WINDOWS_UNIVERSAL)
        DWORD64 offset = (DWORD64)_offset;
        DWORD offsetHigh = (offset >> 32) & 0xFFFFFFFFlu;
        DWORD offsetLow = (offset >> 0) & 0xFFFFFFFFlu;

        DWORD offsetLowGranularity = offsetLow / _dwAllocationGranularity * _dwAllocationGranularity;
        DWORD offsetLowResidue = offsetLow - offsetLowGranularity;

        LPVOID memory = ::MapViewOfFile( _hMapping, FILE_MAP_READ, offsetHigh, offsetLowGranularity, (DWORD)_size + offsetLowResidue );

        if( memory == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid map view of file [error: %lu]"
                , error
            );

            return false;
        }

        m_memoryGranularity = memory;

        m_base = MENGINE_PVOID_OFFSET( m_memoryGranularity, offsetLowResidue );
        m_size = _size;

        m_pos = m_base;
        m_end = m_base + m_size;

        return true;
#else
        MENGINE_UNUSED( _hMapping );
        MENGINE_UNUSED( _dwAllocationGranularity );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _size );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileMappedInputStream::unmap()
    {
        if( ::UnmapViewOfFile( m_memoryGranularity ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid UnmapViewOfFile %p [error: %lu]"
                , m_base
                , error
            );
        }

        m_memoryGranularity = NULL;
        
        m_size = 0;

        m_base = nullptr;
        m_pos = nullptr;
        m_end = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileMappedInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( Win32FileMappedInputStream, this, "Win32FileMappedInputStream::read" );

        size_t cnt = _count;

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

        m_pos = m_base + _pos;

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

        m_pos = m_base + m_size - _pos;

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

        size_t distance = m_pos - m_base;

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
        *_memory = m_base;
        *_size = m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
