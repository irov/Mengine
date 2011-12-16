/*
 *	PosixOutputStream.h
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include <cstdio>

namespace Menge
{
	class PosixOutputStream
		: public OutputStreamInterface
	{
	public:
		PosixOutputStream();
		~PosixOutputStream();

		bool open( const String& _filename );

	protected:
		void write( const void * _data, int _count ) override;
		void flush() override;

	private:
		FILE* m_hFile;
	};
}	// namespace Menge
