/*
 *	FileStream.h
 *
 *	Created by _Berserk_ on 27.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DataStream.h"
#	include <windows.h>

static const DWORD s_fileStreamBufferSize = 512;

namespace Menge
{
	class FileStream 
		: public DataStream
	{
	public:
		FileStream( const StringW& _filename );
		~FileStream();

		bool isValid();
	public:
		void release() override;
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void seek( std::streamoff _pos ) override;
		void skip( std::streamoff _count ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const override;
		bool isMemory() const override;

	protected:
		HANDLE m_handle;
		DWORD m_size;
		DWORD m_readPointer;

		unsigned char m_buffer[s_fileStreamBufferSize];
		DWORD m_bufferSize;
		DWORD m_bufferBegin;
	};
}	// namespace Menge
