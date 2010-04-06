/*
 *	Win32OutputStream.h
 *
 *	Created by _Berserk_ on 18.11.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#include "WindowsLayer/WindowsLayer.h"

namespace Menge
{
	class Win32OutputStream
		: public OutputStreamInterface
	{
	public:
		Win32OutputStream();
		~Win32OutputStream();

		bool open( const String& _filename );
		void close();
	public:
		void write( const void * _data, std::streamsize _count ) override;
		void flush() override;

	private:
		HANDLE m_hFile;
	};
}	// namespace Menge
