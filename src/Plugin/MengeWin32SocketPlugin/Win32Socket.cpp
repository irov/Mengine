#	include "Win32Socket.h"

namespace Menge
{
	Win32Socket::Win32Socket()
		:m_socket( INVALID_SOCKET )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32Socket::~Win32Socket()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Socket::initialize( const Char * _ip, const Char * _port )
	{
		ADDRINFOA hints;
		ZeroMemory( &hints, sizeof( hints ) );

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		PADDRINFOA addrinfo;
		INT getaddrinfo_result = ::getaddrinfo( _ip, _port, &hints, &addrinfo );

		if( getaddrinfo_result != 0 )
		{
			return false;
		}

		SOCKET socket = ::socket( addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol );

		int connect_result = ::connect( socket, addrinfo->ai_addr, addrinfo->ai_addrlen );

		freeaddrinfo( addrinfo );

		if( connect_result != 0 )
		{
			return false;
		}

		m_socket = socket;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Socket::finalize()
	{
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Socket::send( const void * _buffer, size_t _len )
	{
		const char * send_buffer = (const char *)_buffer;

		int send_result = ::send( m_socket, send_buffer, _len, 0 );

		if( send_result != 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Socket::receiv( void * _buffer, size_t _capacity, size_t & _receiv )
	{
		char * receiv_buffer = (char *)_buffer;

		int result = ::recv( m_socket, receiv_buffer, _capacity, 0 );

		if( result < 0 )
		{
			return false;
		}
		
		_receiv = (size_t)result;

		return true;
	}
}