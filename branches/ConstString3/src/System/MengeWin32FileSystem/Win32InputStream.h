/*
 *	Win32InputStream.h
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "WindowsLayer/WindowsLayer.h"

namespace Menge
{
	class Win32InputStream
		: public FileInputStreamInterface
	{
	public:
		Win32InputStream();
		~Win32InputStream();

	public:
		bool open( const String& _filename ) override;
		void close() override;
		
		bool time( time_t & _time ) const override;

	public:
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int tell() const override;
		int size() const override;
		

	private:
		HANDLE m_hFile;
		DWORD m_size;
	};
}	// namespace Menge
