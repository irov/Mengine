#include "Win32Socket.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    Win32Socket::Win32Socket()
        : m_socket( INVALID_SOCKET )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Socket::~Win32Socket()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::connect( const Char * _ip, const Char * _port )
    {
        addrinfo hints;
        ZeroMemory( &hints, sizeof( hints ) );

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo *addrinfo;
        INT getaddrinfo_result = ::getaddrinfo( _ip, _port, &hints, &addrinfo );

        if( getaddrinfo_result != 0 )
        {
            return false;
        }

        SOCKET socket = ::socket( addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol );

		if (socket == 0)
		{
			return false;
		}

        int connect_result = ::connect( socket, addrinfo->ai_addr, (int)addrinfo->ai_addrlen );

        freeaddrinfo( addrinfo );

        if( connect_result != 0 )
        {
            return false;
        }

        m_socket = socket;

		m_receiveStream = new FactorableUnique<Win32SocketInputStream>(socket);
		m_sendStream = new FactorableUnique<Win32SocketOutputStream>(socket);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Socket::disconnect()
    {
        closesocket( m_socket );
        m_socket = INVALID_SOCKET;
    }
    //////////////////////////////////////////////////////////////////////////
	const OutputStreamInterfacePtr & Win32Socket::getSendStream() const
	{
		return m_sendStream;
	}
	//////////////////////////////////////////////////////////////////////////
	const InputStreamInterfacePtr & Win32Socket::getReceiveStream() const
	{
		return m_receiveStream;
	}
}