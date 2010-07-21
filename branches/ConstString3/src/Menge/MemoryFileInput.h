/*
 *	MemoryFileInput.h
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "FileInput.h"

namespace Menge
{
	class MemoryFileInput
		: public FileInput
	{
	public:
		MemoryFileInput();
		~MemoryFileInput();

	public:
		void setMemory( void* _memPtr, int _size );
		void* getMemory();

	public:
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int size() const override;
		int tell() const override;
		bool time( time_t & _time ) const override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		int m_size;
	};
}	// namespace Menge


