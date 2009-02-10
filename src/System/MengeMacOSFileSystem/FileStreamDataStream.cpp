#	include "FileStreamDataStream.h"

#	include <assert.h>
#	include <fstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::FileStreamDataStream( std::ifstream* _s, std::streamsize _size )
		: m_stream( _s )
		, m_size( _size )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::~FileStreamDataStream()
	{
		if ( m_stream )
		{
			m_stream->close();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStreamDataStream::read( void* _buf, std::streamsize _count )
	{
		m_stream->read( static_cast<char*>(_buf), _count );
		return m_stream->gcount();
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::skip( std::streamoff _count )
	{
#if defined(STLPORT)
		// Workaround for STLport issues: After reached eof of file stream,
		// it's seems the stream was putted in intermediate state, and will be
		// fail if try to repositioning relative to current position.
		// Note: tellg() fail in this case too.
		if (m_stream->eof())
		{
			m_stream->clear();
			// Use seek relative to either begin or end to bring the stream
			// back to normal state.
			m_stream->seekg(0, std::ios::end);
		}
#endif 		
		m_stream->clear(); //Clear fail status in case eof was set
		m_stream->seekg( _count, std::ios::cur );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::seek( std::streamoff _pos )
	{
		m_stream->clear(); //Clear fail status in case eof was set
		m_stream->seekg( _pos, std::ios::beg );
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos FileStreamDataStream::tell() const
	{
		m_stream->clear(); //Clear fail status in case eof was set
		return m_stream->tellg();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileStreamDataStream::eof() const
	{
		return m_stream->eof();
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStreamDataStream::size() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
}