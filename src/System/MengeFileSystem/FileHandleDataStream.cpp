#	include "FileHandleDataStream.h"

#	include <assert.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileHandleDataStream::FileHandleDataStream( FILE* _handle )
		: DataStream()
		, m_fileHandle( _handle )
	{
		// Determine size
		fseek( m_fileHandle, 0, SEEK_END );
		m_size = ftell( m_fileHandle );
		fseek( m_fileHandle, 0, SEEK_SET );
	}
	//////////////////////////////////////////////////////////////////////////
	FileHandleDataStream::FileHandleDataStream( const Menge::String& _name, FILE* _handle )
		: DataStream( _name )
		, m_fileHandle( _handle )
	{
		// Determine size
		fseek( m_fileHandle, 0, SEEK_END );
		m_size = ftell( m_fileHandle );
		fseek( m_fileHandle, 0, SEEK_SET );
	}
	//////////////////////////////////////////////////////////////////////////
	FileHandleDataStream::~FileHandleDataStream()
	{
		close();
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileHandleDataStream::read( void* _buf, std::streamsize _count )
	{
		return fread( _buf, 1, _count, m_fileHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::skip( std::streampos _count )
	{
		fseek( m_fileHandle, _count, SEEK_CUR);
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::seek( std::streamoff _pos )
	{
		fseek(m_fileHandle, _pos, SEEK_SET);
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos FileHandleDataStream::tell() const
	{
		return ftell( m_fileHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileHandleDataStream::eof() const
	{
		return feof(m_fileHandle) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileHandleDataStream::close()
	{
		fclose( m_fileHandle );
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
