#pragma once

#include "Interface/StreamInterface.h"

#pragma warning(push, 0)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#pragma warning(pop)

namespace Mengine
{
	class Win32SocketOutputStream
		: public OutputStreamInterface
	{
	public:
		Win32SocketOutputStream(SOCKET _socket);
		~Win32SocketOutputStream() override;

	public:
		size_t write( const void * _data, size_t _size ) override;
		size_t size() const override;

	public:
		bool flush() override;

	protected:
		SOCKET m_socket;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Win32SocketOutputStream, OutputStreamInterface> Win32SocketOutputStreamPtr;
	//////////////////////////////////////////////////////////////////////////
}