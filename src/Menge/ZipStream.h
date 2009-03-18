/*
 *	ZipStream.h
 *
 *	Created by _Berserk_ on 16.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "zlib.h"

#	define ZIP_STREAM_BUFFER 1024

namespace Menge
{
	class ZipStream
		: public DataStreamInterface
	{
	public:
		ZipStream( DataStreamInterface* _stream		// DataStreamInterface asumed to be shared
			, std::streampos _begin
			, std::streamsize _size
			, std::streamsize _unzSize );
		~ZipStream();

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
		std::streampos m_streamBegin;
		std::streamsize m_streamSize;
		std::streamsize m_size;

		std::streampos m_currentStreamPos;
		std::streampos m_unzPos;
		z_stream m_zStream;
		bool m_valid;
		unsigned char m_buffer[ZIP_STREAM_BUFFER];

		void readNextPortion_();
	};
}	// namespace Menge
