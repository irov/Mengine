#	pragma once

#	include "Interface/SocketInterface.h"

#	include <winsock2.h> 
#	include <ws2tcpip.h>

namespace Menge
{
	class Win32Socket
		: public SocketInterface
	{
	public:
		Win32Socket();
		~Win32Socket();

	public:
		bool initialize( const Char * _ip, const Char * _port );
		void finalize();
		
	public:
		bool send( const void * _buffer, size_t _len );
		bool receiv( void * _buffer, size_t _capacity, size_t & _receiv );

	protected:
		SOCKET m_socket;		
	};
}