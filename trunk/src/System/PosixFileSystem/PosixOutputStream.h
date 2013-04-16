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
		: public FileOutputStreamInterface
	{
	public:
		PosixOutputStream();
		~PosixOutputStream();

    public:
		bool open( const String& _filename ) override;

	protected:
		void write( const void * _data, int _count ) override;
		void flush() override;

    protected:
        void destroy() override;

	private:
		FILE* m_hFile;
	};
}	// namespace Menge
