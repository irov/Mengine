#include "PosixSocketOutputStream.h"

#include "Kernel/AssertionNotImplemented.h"

#include <sys/socket.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PosixSocketOutputStream::PosixSocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PosixSocketOutputStream::~PosixSocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PosixSocketOutputStream::write( const void * _data, size_t _size )
    {
        const char * send_data = (const char *)_data;

        SOCKET socket = this->getSocket();

        int32_t send_result = ::send( socket, send_data, (int32_t)_size, 0 );

        if( send_result < 0 )
        {
            return false;
        }

        return (size_t)send_result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t PosixSocketOutputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocketOutputStream::flush()
    {
        return true;
    }
}