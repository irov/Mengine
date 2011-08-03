/*
 *	FileStream.h
 *
 *	Created by _Berserk_ on 27.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Interface/FileSystemInterface.h"
#	include <windows.h>

static const DWORD s_fileStreamBufferSize = 512;

namespace Menge
{
	class FileStream 
		: public DataStreamInterface
	{
	public:
		FileStream();
		~FileStream();

	public:
		bool initialize( const StringW& _filename, bool _map );

	public:
		bool isValid();

	public:
		void release() override;
		std::streamsize read( void* _buf, std::streamsize _count ) override;
		void seek( std::streamoff _pos ) override;
		void skip( std::streamoff _count ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const override;

	protected:
		HANDLE m_handle;
		DWORD m_size;
		DWORD m_readPointer;

		//unsigned char m_buffer[s_fileStreamBufferSize];
		unsigned char* m_buffer;
		DWORD m_bufferSize;
		DWORD m_bufferBegin;

		bool m_mapped;
		HANDLE m_mappingHandle;
	};
}	// namespace Menge
