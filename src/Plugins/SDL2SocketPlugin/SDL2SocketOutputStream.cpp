#include "SDLSocketOutputStream.h"

#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLSocketOutputStream::SDLSocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSocketOutputStream::~SDLSocketOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLSocketOutputStream::write( const void * _data, size_t _size )
    {
        TCPsocket socket = this->getSocket();

        int sent = SDLNet_TCP_Send( socket, _data, static_cast<int>(_size) );

        if( sent < 0 )
        {
            return 0;
        }

        return (size_t)sent;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLSocketOutputStream::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocketOutputStream::flush()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}