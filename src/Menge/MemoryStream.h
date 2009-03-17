/*
 *	MemoryStream.h
 *
 *	Created by _Berserk_ on 17.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class MemoryStream
		: public DataStreamInterface
	{
	public:
		MemoryStream( void* _pMem, std::streamsize _size );
		~MemoryStream();

	public:
		void release() override;
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void skip( std::streamoff _count ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const override;

	protected:
		unsigned char* m_data;
		unsigned char* m_pos;
		unsigned char* m_end;
		std::streamsize m_size;
	};
}	// namespace Menge
