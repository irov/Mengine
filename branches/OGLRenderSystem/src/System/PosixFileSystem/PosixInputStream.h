/*
 *	PosixInputStream.h
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#include <sys/stat.h>

namespace Menge
{
	class PosixInputStream
		: public InputStreamInterface
	{
	public:
		PosixInputStream();
		~PosixInputStream();

		bool open( const String& _filename );
		void close();

	public:
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override; 

	private:
		int m_hFile;
		off_t m_size;
	};
}	// namespace Menge
