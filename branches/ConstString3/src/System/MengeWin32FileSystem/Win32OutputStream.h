/*
 *	Win32OutputStream.h
 *
 *	Created by _Berserk_ on 18.11.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "WindowsLayer/WindowsLayer.h"

#	include <time.h>

namespace Menge
{
	class Win32OutputStream
		: public FileOutputStreamInterface
	{
	public:
		Win32OutputStream();
		~Win32OutputStream();

	public:
		bool open( const WString& _filename ) override;
		void close() override;

		int tell() override;

	public:
		void write( const void * _data, int _count ) override;
		void flush() override;

	private:
		HANDLE m_hFile;
	};
}	// namespace Menge
