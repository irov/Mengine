#include "Win32SocketOutputStream.h"

#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SocketOutputStream::Win32SocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SocketOutputStream::~Win32SocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32SocketOutputStream::write( const void * _data, size_t _size )
    {
        const char * send_data = (const char *)_data;

        SOCKET socket = this->getSocket();

        int32_t send_result = ::send( socket, send_data, (int32_t)_size, 0 );

        if( send_result < 0 )
        {
            return 0;
        }

        return (size_t)send_result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32SocketOutputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketOutputStream::flush()
    {
        //Empty

        return true;
    }
}