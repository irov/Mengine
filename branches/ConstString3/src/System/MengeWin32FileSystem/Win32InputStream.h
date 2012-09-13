/*
 *	Win32InputStream.h
 *
 *	Created by _Berserk_ on 26.8.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "WindowsLayer/WindowsLayer.h"

namespace Menge
{
    const size_t FILE_BUFFER_SIZE = 8192; //8kb

	class Win32InputStream
		: public FileInputStreamInterface
	{
	public:
		Win32InputStream( LogServiceInterface * _logService );
		~Win32InputStream();

	public:
		bool open( const WString& _filename ) override;
		void close() override;
		
		bool time( time_t & _time ) const override;

	public:
		int read( void* _buf, int _count ) override;
		void seek( int _pos ) override;
		int tell() const override;
		int size() const override;		

	private:
        LogServiceInterface * m_logService;

		HANDLE m_hFile;
		DWORD m_size;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;
        char m_buff[FILE_BUFFER_SIZE];
	};
}	// namespace Menge
