/*
 *	Win32OutputStream.h
 *
 *	Created by _Berserk_ on 18.11.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#	include <windows.h>

namespace Menge
{
	class Win32OutputStream
		: public OutputStreamInterface
	{
	public:
		Win32OutputStream();
		~Win32OutputStream();

		bool open( const StringW& _filename );
		void close();
	public:
		void write( const void * _data, int _count ) override;
		void flush() override;

	private:
		HANDLE m_hFile;
	};
}	// namespace Menge
