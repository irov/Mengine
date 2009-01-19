#	include "FileStreamDataStream.h"

#	include <assert.h>
#	include <fstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::FileStreamDataStream( std::ifstream* _s, std::streamsize _size, bool _freeOnClose )
		: DataStream()
		, m_stream( _s )
		, m_freeOnClose( _freeOnClose )
		, m_freeBuffer( true )
		, m_buffer( NULL )
	{
		m_size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::~FileStreamDataStream()
	{
		close();
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
	void FileStreamDataStream::close()
	{
		if ( m_stream )
		{
			m_stream->close();
			if ( m_freeOnClose )
			{
				// delete the stream too
				delete m_stream;
				m_stream = 0;
			}
		}
		if( m_freeBuffer && m_buffer )
		{
			delete[] m_buffer;
			m_buffer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void* FileStreamDataStream::getBuffer()
	{
		m_buffer = new unsigned char[m_size];
		seek( 0 );
		read( m_buffer, m_size );
		return m_buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::setFreeOnClose( bool _free )
	{
		m_freeBuffer = _free;
	}
	//////////////////////////////////////////////////////////////////////////
}
