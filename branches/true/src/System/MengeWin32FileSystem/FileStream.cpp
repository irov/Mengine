/*
 *	FileStream.cpp
 *
 *	Created by _Berserk_ on 27.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStream::FileStream()
		: m_handle( INVALID_HANDLE_VALUE )
		, m_readPointer( 0 )
		, m_buffer( NULL )
		, m_bufferSize( 0 )
		, m_bufferBegin( 0 )
		, m_size( 0 )
		, m_mapped( false )
		, m_mappingHandle( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStream::initialize( const StringW& _filename, bool _map )
	{
		if( _filename.empty() )
		{
			return false;
		}
		
		DWORD shareAttrib = /*m_mapped ? 0 :*/ FILE_SHARE_READ;
		m_handle = CreateFile( _filename.c_str(),    // file to open
			GENERIC_READ,			// open for reading
			shareAttrib,			// share for reading, exclusive for mapping
			NULL,					// default security
			OPEN_EXISTING,			// existing file only
			FILE_ATTRIBUTE_NORMAL,	// normal file
			NULL);					// no attr. template

		if ( m_handle == INVALID_HANDLE_VALUE)
		{
			//LOG_ERROR("Error while opening file " + _filename );
			//return NULL;
			return false;
		}

		m_size = GetFileSize( m_handle, NULL );

		if( m_size == INVALID_FILE_SIZE )
		{
			m_size = 0;

			CloseHandle( m_handle );
			m_handle = INVALID_HANDLE_VALUE;

			return false;
		}

		if( m_mapped == true )
		{
			m_mappingHandle = CreateFileMapping( m_handle, NULL, PAGE_READONLY, 0, 0, NULL );

			if( m_mappingHandle == NULL )
			{
				CloseHandle( m_handle );
				m_handle = INVALID_HANDLE_VALUE;

				return false;
			}

			m_buffer = (unsigned char*)MapViewOfFile( m_mappingHandle, FILE_MAP_READ, 0, 0, m_size );
			m_bufferSize = m_size;
			m_readPointer = m_size;
		}
		else
		{
			m_buffer = new unsigned char[s_fileStreamBufferSize];
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	FileStream::~FileStream()
	{
		if( m_mappingHandle != NULL )
		{
			if( m_buffer != NULL )
			{
				UnmapViewOfFile( m_buffer );
			}

			CloseHandle( m_mappingHandle );
		}
		else
		{
			if( m_buffer != NULL )
			{
				delete m_buffer;
				m_buffer = NULL;
			}
		}

		CloseHandle( m_handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStream::read( void* _buf, std::streamsize _count )
	{
		DWORD bytesRead = 0;
		unsigned char* dstBuf = (unsigned char*)_buf;

		// check how much data we have
		DWORD bytesLeft = m_bufferSize - m_bufferBegin + m_size - m_readPointer;
		if( bytesLeft < _count )
		{
			_count = bytesLeft;
		}

		while( _count > 0 )
		{
			// buffer is empty - fill it
			if( m_bufferBegin == m_bufferSize )
			{
				DWORD readSize = (std::min)( s_fileStreamBufferSize, m_size - m_readPointer );
				BOOL result = ReadFile( m_handle, m_buffer, readSize, &m_bufferSize, NULL );
				m_readPointer += m_bufferSize;
				m_bufferBegin = 0;
				if( result == FALSE )
				{
					// something wrong
					return bytesRead;
				}
			}

			// copy buffer data
			DWORD bytesToCopy = (std::min<DWORD>)( m_bufferSize - m_bufferBegin, _count );
			std::copy( m_buffer + m_bufferBegin
				, m_buffer + m_bufferBegin + bytesToCopy
				, dstBuf );
			m_bufferBegin += bytesToCopy;
			dstBuf += bytesToCopy;
			bytesRead += bytesToCopy;
			_count -= bytesToCopy;
		}

		return bytesRead;

		//if( m_readPointer + _count > m_size )
		//{
		//	_count = m_size - m_readPointer;
		//}
		//BOOL result = ReadFile( m_handle, _buf, _count, &bytesRead, NULL );
		//m_readPointer += bytesRead;
		//return bytesRead;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::seek( std::streamoff _pos )
	{
		if( _pos > m_readPointer
			|| _pos < m_readPointer - m_bufferSize )
		{
			DWORD newPos = SetFilePointer( m_handle, _pos, NULL, FILE_BEGIN );
			if( newPos != INVALID_SET_FILE_POINTER )
			{
				// set new pointer and drop buffer
				m_readPointer = newPos;
				m_bufferSize = 0;
				m_bufferBegin = 0;
			}
		}
		else	// we're still in buffer, just fix m_bufferBegin
		{
			m_bufferBegin = _pos - m_readPointer + m_bufferSize;
		}

		//DWORD newPos = SetFilePointer( m_handle, _pos, NULL, FILE_BEGIN );
		//if( GetLastError() == NO_ERROR )
		//{
		//	m_readPointer = newPos;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStream::skip( std::streamoff _count )
	{
		m_bufferBegin += _count;
		if( m_bufferBegin < 0
			|| m_bufferBegin > m_bufferSize ) // we're out of buffer
		{
			// calc new position and drop buffer
			DWORD pos = m_readPointer - m_bufferSize + m_bufferBegin;
			DWORD newPos = SetFilePointer( m_handle, pos, NULL, FILE_BEGIN );
			if( newPos != INVALID_SET_FILE_POINTER )
			{
				// set new pointer and drop buffer
				m_readPointer = newPos;
				m_bufferSize = 0;
				m_bufferBegin = 0;
			}
			else	// some error - restore state
			{
				DWORD error = GetLastError();
				m_bufferBegin -= _count;
			}
		}
		//DWORD newPos = SetFilePointer( m_handle, _count, NULL, FILE_CURRENT );
		//if( GetLastError() == NO_ERROR )
		//{
		//	m_readPointer = newPos;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos FileStream::tell() const
	{
		//return m_readPointer;
		return ( m_readPointer - m_bufferSize + m_bufferBegin );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStream::eof() const
	{
		return ( m_readPointer - m_bufferSize + m_bufferBegin ) == m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStream::size() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStream::isValid()
	{
		if ( m_handle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
