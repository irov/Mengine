/*
 *	WrapStream.h
 *
 *	Created by _Berserk_ on 18.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class WrapStream
		: public DataStreamInterface
	{
	public:
		WrapStream( DataStreamInterface* _stream, std::streampos _begin, std::streamsize _size );
		~WrapStream();

	public:
		void release() override;
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void seek( std::streamoff _pos ) override;
		void skip( std::streamoff _count ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const override;

	protected:
		DataStreamInterface* m_stream;
		std::streamoff m_streamBegin;
		std::streamsize m_size;
		std::streamoff m_currentPos;
	};
}	// namespace Menge
