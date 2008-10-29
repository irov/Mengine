#	include "FileHandleDataStream.h"

#	include <cassert>
#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileHandleDataStream::FileHandleDataStream( FILE* _handle )
		: DataStream()
		, m_fileHandle( _handle )
	{
		// Determine size
		std::fseek( m_fileHandle, 0, SEEK_END );
		m_size = std::ftell( m_fileHandle );
		std::fseek( m_fileHandle, 0, SEEK_SET );
	}
	//////////////////////////////////////////////////////////////////////////
	FileHandleDataStream::~FileHandleDataStream()
	{
		close();
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileHandleDataStream::read( void* _buf, std::streamsize _count )
	{
		return std::fread( _buf, 1, _count, m_fileHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::skip( std::streamoff _count )
	{
		std::fseek( m_fileHandle, _count, SEEK_CUR);
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::seek( std::streamoff _pos )
	{
		std::fseek(m_fileHandle, _pos, SEEK_SET);
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos FileHandleDataStream::tell() const
	{
		return std::ftell( m_fileHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileHandleDataStream::eof() const
	{
		return std::feof(m_fileHandle) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::close()
	{
		std::fclose( m_fileHandle );
		m_fileHandle = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void* FileHandleDataStream::getBuffer()
	{
		assert( 0 && "Not Implemented" );
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::setFreeOnClose( bool _free )
	{
		// nothing to do
	}
}
