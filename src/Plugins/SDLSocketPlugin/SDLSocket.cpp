#include "SDLSocket.h"

#include "Kernel/Stringalized.h"

namespace Mengine
{
    SDLSocket::SDLSocket()
        : m_socket( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSocket::~SDLSocket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::connect( const SocketConnectInfo & _info )
    {
        Uint16 intPort;
        Helper::stringalized( _info.port, &intPort );

        IPaddress addr;
        if( SDLNet_ResolveHost( &addr, _info.ip, intPort ) < 0 )
        {
            return false;
        }

        TCPsocket socket = SDLNet_TCP_Open( &addr );

        if( socket == nullptr )
        {
            return false;
        }

        m_socket = socket;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::bind( const SocketConnectInfo & _info, bool _blocking )
    {
        MENGINE_UNUSED( _info );
        MENGINE_UNUSED( _blocking );

        //ToDo

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSocket::disconnect()
    {
        if( m_socket != nullptr )
        {
            SDLNet_TCP_Close( m_socket );
            m_socket = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLSocket::checkForClientConnection()
    {
        //ToDo

        return -1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::waitForData( uint32_t _timeoutMs )
    {
        MENGINE_UNUSED( _timeoutMs );

        //ToDo

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::send( const void * _buffer, size_t _size, size_t * const _sent )
    {
        int result = SDLNet_TCP_Send( m_socket, _buffer, static_cast<int>(_size) );

        if( result < static_cast<int>(_size) )
        {
            return false;
        }

        *_sent = static_cast<size_t>(result);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::receive( void * _buffer, size_t _capacity, size_t * const _received )
    {
        int result = SDLNet_TCP_Recv( m_socket, _buffer, static_cast<int>(_capacity) );

        if( result < 0 )
        {
            return false;
        }

        *_received = static_cast<size_t>(result);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr SDLSocket::getSendStream() const
    {
        return OutputStreamInterfacePtr( this );
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLSocket::getReceiveStream() const
    {
        return InputStreamInterfacePtr( this );
    }
    //////////////////////////////////////////////////////////////////////////
    TCPsocket SDLSocket::getSocket() const
    {
        return m_socket;
    }
    //////////////////////////////////////////////////////////////////////////
}
