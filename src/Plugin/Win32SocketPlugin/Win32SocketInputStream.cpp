#include "Win32SocketInputStream.h"

#include "Kernel/NotImplementedAssertion.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Win32SocketInputStream::Win32SocketInputStream(SOCKET _socket)
		: m_socket(_socket)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32SocketInputStream::~Win32SocketInputStream()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32SocketInputStream::read( void * _buffer, size_t _size )
	{
		char * receiv_buffer = (char *)_buffer;

		int result = ::recv(m_socket, receiv_buffer, (int)_size, 0);

		if (result < 0)
		{
			return 0;
		}

		return (size_t)result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketInputStream::seek(size_t _pos)
	{
		(void)_pos;

		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketInputStream::skip(size_t _pos)
	{
		(void)_pos;

		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32SocketInputStream::tell() const
	{
		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32SocketInputStream::size() const
	{
		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketInputStream::eof() const
	{
		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketInputStream::time(uint64_t & _time) const
	{
		(void)_time;

		MENGINE_ASSERTION_NOT_IMPLEMENTED();

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32SocketInputStream::memory(void ** _memory, size_t * _size)
	{
		(void)_memory;
		(void)_size;

		//Empty

		return false;
	}
}