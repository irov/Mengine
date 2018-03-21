#pragma once

#include "Interface/SocketInterface.h"

#include "Utils/WIN32/WindowsIncluder.h"

#	pragma warning(push, 0) 
#include <winsock2.h>
#include <ws2tcpip.h>
#	pragma warning(pop) 


namespace Mengine
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
		bool receive( void * _buffer, size_t _capacity, size_t & _receiv );

	protected:
		SOCKET m_socket;		
	};
}