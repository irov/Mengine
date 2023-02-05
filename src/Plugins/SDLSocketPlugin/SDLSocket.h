#pragma once

#include "Interface/SocketInterface.h"

#include "SDLSocketInputStream.h"
#include "SDLSocketOutputStream.h"

#include "SDL_net.h"

namespace Mengine
{
    class SDLSocket
        : public SocketInterface
        , public SDLSocketInputStream
        , public SDLSocketOutputStream
    {
        DECLARE_FACTORABLE( SDLSocket );

    public:
        SDLSocket();
        ~SDLSocket() override;

    public:
        bool initialize( const Char * _ip, const Char * _port );
        void finalize();
        
    public:
        bool connect( const SocketConnectInfo & _data ) override;
        bool bind( const SocketConnectInfo & _data, bool _blocking = true ) override;
        void disconnect() override;

        int32_t checkForClientConnection() override;
        bool waitForData( uint32_t _timeoutMs ) override;

    public:
        bool send( const void * _buffer, size_t _size, size_t * const _sent );
        bool receive( void * const _buffer, size_t _capacity, size_t * const _received );

    public:
        OutputStreamInterfacePtr getSendStream() const override;
        InputStreamInterfacePtr getReceiveStream() const override;

    protected:
        TCPsocket getSocket() const override;

    protected:
        TCPsocket m_socket;
    };
}
