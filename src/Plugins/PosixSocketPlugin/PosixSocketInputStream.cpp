#include "PosixSocketInputStream.h"

#include "Kernel/AssertionNotImplemented.h"

#include <sys/socket.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PosixSocketInputStream::PosixSocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PosixSocketInputStream::~PosixSocketInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PosixSocketInputStream::read( void * _buffer, size_t _size )
    {
        char * receiv_buffer = (char *)_buffer;

        SOCKET socket = this->getSocket();

        int32_t result = ::recv( socket, receiv_buffer, (int32_t)_size, 0 );

        if( result < 0 )
        {
            return 0;
        }

        return (size_t)result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketInputStream::seek( size_t _pos )
    {
        (void)_pos;

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketInputStream::skip( size_t _pos )
    {
        (void)_pos;

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PosixSocketInputStream::tell() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PosixSocketInputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketInputStream::eof() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketInputStream::time( uint64_t & _time ) const
    {
        (void)_time;

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketInputStream::memory( void ** _memory, size_t * _size )
    {
        (void)_memory;
        (void)_size;

        //Empty

        return false;
    }
}