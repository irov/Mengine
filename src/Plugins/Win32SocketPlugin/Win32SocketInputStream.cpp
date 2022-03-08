#include "Win32SocketInputStream.h"

#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SocketInputStream::Win32SocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SocketInputStream::~Win32SocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32SocketInputStream::read( void * const _buffer, size_t _size )
    {
        char * receiv_buffer = static_cast<char *>(_buffer);

        SOCKET socket = this->getSocket();

        int32_t result = ::recv( socket, receiv_buffer, (int32_t)_size, 0 );

        if( result < 0 )
        {
            return 0;
        }

        return (size_t)result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::seek( size_t _pos )
    {
        MENGINE_UNUSED( _pos );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::rseek( size_t _pos )
    {
        MENGINE_UNUSED( _pos );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::skip( size_t _size )
    {
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32SocketInputStream::tell() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32SocketInputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::eof() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::time( uint64_t * _time ) const
    {
        MENGINE_UNUSED( _time );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}