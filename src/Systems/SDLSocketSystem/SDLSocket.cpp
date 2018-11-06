#	include "SDLSocket.h"

namespace Mengine
{
    SDLSocket::SDLSocket()
        : m_socket(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSocket::~SDLSocket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::initialize( const Char * _ip, const Char * _port )
    {
        IPaddress addr;
        const Uint16 intPort = static_cast<Uint16>(atoi(_port));
        if (SDLNet_ResolveHost(&addr, _ip, intPort) < 0)
        {
            return false;
        }

        m_socket = SDLNet_TCP_Open(&addr);

        return (m_socket != nullptr);
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSocket::finalize()
    {
        SDLNet_TCP_Close(m_socket);
        m_socket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::send( const void * _buffer, size_t _len )
    {
        const int result = SDLNet_TCP_Send(m_socket, _buffer, static_cast<int>(_len));

        if (result < static_cast<int>(_len))
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSocket::receive( void * _buffer, size_t _capacity, size_t & _receiv )
    {
        const int result = SDLNet_TCP_Recv(m_socket, _buffer, static_cast<int>(_capacity));

        if (result < 0)
        {
            return false;
        }
        
        _receiv = static_cast<size_t>(result);

        return true;
    }
}
