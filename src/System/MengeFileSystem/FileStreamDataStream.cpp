#	include "FileStreamDataStream.h"

#	include <assert.h>
#	include <fstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::FileStreamDataStream( std::ifstream* _s, bool _freeOnClose )
		: DataStream()
		, m_stream( _s )
		, m_freeOnClose( _freeOnClose )
		, m_freeBuffer( true )
		, m_buffer( NULL )
	{
		// calculate the size
		m_stream->seekg( 0, std::ios_base::end );
		m_size = m_stream->tellg();
		m_stream->seekg( 0, std::ios_base::beg );

	}
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::FileStreamDataStream( const String& _name, std::ifstream* _s, bool _freeOnClose )
		: DataStream( _name )
		, m_stream( _s )
		, m_freeOnClose( _freeOnClose )
		, m_freeBuffer( true )
		, m_buffer( NULL )
	{
		// calculate the size
		m_stream->seekg( 0, std::ios_base::end );
		m_size = m_stream->tellg();
		m_stream->seekg( 0, std::ios_base::beg );
	}
	//////////////////////////////////////////////////////////////////////////
	FileStreamDataStream::FileStreamDataStream( const String& _name,	std::ifstream* _s, std::streamsize _size, bool _freeOnClose )
		: DataStream( _name )
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
	std::streamsize FileStreamDataStream::read( void* _buf, std::streamsize _count )
	{
		m_stream->read( static_cast<char*>(_buf), _count );
		return m_stream->gcount();
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize FileStreamDataStream::readLine( char* _buf, std::streamsize _maxCount, const String& _delim )
	{
		assert( !_delim.empty() &&  "FileStreamDataStream::readLine -> No delimiter provided" );

		/*if (delim.size() > 1)
		{
		LogManager::getSingleton().logMessage(
		"WARNING: FileStreamDataStream::readLine - using only first delimeter");
		}*/

		// Deal with both Unix & Windows LFs
		bool trimCR = false;
		if ( _delim.at(0) == MENGE_TEXT('\n') ) 
		{
			trimCR = true;
		}
		// _maxCount + 1 since count excludes terminator in getline
		m_stream->getline( _buf, _maxCount + 1, _delim.at( 0 ) );
		std::streamsize ret = m_stream->gcount();
		// three options
		// 1) we had an eof before we read a whole line
		// 2) we ran out of buffer space
		// 3) we read a whole line - in this case the delim character is taken from the stream but not written in the buffer so the read data is of length ret-1 and thus ends at index ret-2
		// in all cases the buffer will be null terminated for us

		if ( m_stream->eof() ) 
		{
			// no problem
		}
		else if ( m_stream->fail() )
		{
			// Did we fail because of maxCount hit? No - no terminating character
			// in included in the count in this case
			if ( ret == _maxCount )
			{
				// clear failbit for next time 
				m_stream->clear();
			}
			else
			{
				/*MENGE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
				"Streaming error occurred", 
				"FileStreamDataStream::readLine");*/
			}
		}
		else 
		{
			// we need to adjust ret because we want to use it as a
			// pointer to the terminating null character and it is
			// currently the length of the data read from the stream
			// i.e. 1 more than the length of the data in the buffer and
			// hence 1 more than the _index_ of the NULL character
			--ret;
		}

		// trim off CR if we found CR/LF
		if ( trimCR && _buf[ret-1] == MENGE_TEXT('\r') )
		{
			--ret;
			_buf[ret] = MENGE_TEXT('\0');
		}
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::skip( std::streampos _count )
	{
#if defined(STLPORT)
		// Workaround for STLport issues: After reached eof of file stream,
		// it's seems the stream was putted in intermediate state, and will be
		// fail if try to repositioning relative to current position.
		// Note: tellg() fail in this case too.
		if (mpStream->eof())
		{
			mpStream->clear();
			// Use seek relative to either begin or end to bring the stream
			// back to normal state.
			mpStream->seekg(0, std::ios::end);
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
	bool FileStreamDataStream::isMemory() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileStreamDataStream::setFreeOnClose( bool _free )
	{
		m_freeBuffer = _free;
	}
}