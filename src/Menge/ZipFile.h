/*
 *	ZipFile.h
 *
 *	Created by _Berserk_ on 16.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class ZipFile
		: DataStreamInterface
	{
	public:
		ZipFile( DataStreamInterface* _stream );
		~ZipFile();

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
	};
}	// namespace Menge
