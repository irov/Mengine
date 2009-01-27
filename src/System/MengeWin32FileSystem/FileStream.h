/*
 *	FileStream.h
 *
 *	Created by _Berserk_ on 27.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Interface/FileSystemInterface.h"
#	include <windows.h>

namespace Menge
{
	class FileStream 
		: public DataStreamInterface
	{
	public:
		FileStream( HANDLE _handle );
		~FileStream();

		HANDLE getHandle();
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
	};
}	// namespace Menge
