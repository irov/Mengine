#   pragma once

#   include "Interface/SocketInterface.h"

#   include "SDL_net.h"

namespace Menge
{
    class SDLSocket
        : public SocketInterface
    {
    public:
        SDLSocket();
        ~SDLSocket();

    public:
        bool initialize( const Char * _ip, const Char * _port );
        void finalize();
        
    public:
        bool send( const void * _buffer, size_t _len );
        bool receive( void * _buffer, size_t _capacity, size_t & _receiv );

    protected:
        TCPsocket m_socket;
    };
}
