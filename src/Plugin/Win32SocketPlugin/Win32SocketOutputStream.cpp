#include "Win32SocketOutputStream.h"

#include "Kernel/NotImplementedAssertion.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Win32SocketOutputStream::Win32SocketOutputStream(SOCKET _socket)
		: m_socket(_socket)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32SocketOutputStream::~Win32SocketOutputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32SocketOutputStream::write( const void * _data, size_t _size )
	{
		const char * send_data = (const char *)_data;

		int send_result = ::send(m_socket, send_data, (int)_size, 0);

		if (send_result < 0)
		{
			return false;
		}

		return (size_t)send_result;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32SocketOutputStream::size() const
	{
		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketOutputStream::flush()
	{
		return true;
	}
}