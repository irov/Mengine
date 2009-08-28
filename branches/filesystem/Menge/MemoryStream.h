/*
 *	MemoryStream.h
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInterface.h"

namespace Menge
{
	class MemoryStream
		: public FileInterface
	{
	public:
		MemoryStream( void* _pMem, int _size );
		~MemoryStream();

	public:
		void close() override;
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		int m_size;
	};
}	// namespace Menge
