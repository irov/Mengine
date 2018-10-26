#pragma once

#include "Interface/StreamInterface.h"

#pragma warning(push, 0)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#pragma warning(pop)

namespace Mengine
{
	class Win32SocketInputStream
		: public InputStreamInterface
	{
	public:
		Win32SocketInputStream(SOCKET _socket);
		~Win32SocketInputStream() override;

	public:
		size_t read(void * _buffer, size_t _size) override;
		bool seek(size_t _pos) override;
		bool skip(size_t _pos) override;
		size_t tell() const override;
		size_t size() const override;
		bool eof() const override;

	public:
		bool time(uint64_t & _time) const override;

	public:
		bool memory(void ** _memory, size_t * _size) override;

	protected:
		SOCKET m_socket;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Win32SocketInputStream, InputStreamInterface> Win32SocketInputStreamPtr;
	//////////////////////////////////////////////////////////////////////////
}