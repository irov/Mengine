#pragma once

#include "Interface/InputStreamInterface.h"

#include "Win32SocketProviderInterface.h"

namespace Mengine
{
	class Win32SocketInputStream
		: public InputStreamInterface
        , virtual public Win32SocketProviderInterface
	{
	public:
		Win32SocketInputStream();
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
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Win32SocketInputStream, InputStreamInterface> Win32SocketInputStreamPtr;
	//////////////////////////////////////////////////////////////////////////
}