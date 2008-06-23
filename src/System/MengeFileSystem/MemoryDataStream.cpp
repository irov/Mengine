#	include "MemoryDataStream.h"

#	include <assert.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( void* _pMem, std::size_t _size, bool _freeOnClose )
		: DataStream()
	{
		m_data = m_pos = static_cast<unsigned char*>(_pMem);
		m_size = _size;
		m_end = m_data + m_size;
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( const Menge::String& _name, void* _pMem, std::size_t _size, bool _freeOnClose)
		: DataStream( _name )
	{
		m_data = m_pos = static_cast<unsigned char*>(_pMem);
		m_size = _size;
		m_end = m_data + m_size;
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( DataStream& _sourceStream, bool _freeOnClose )
		: DataStream()
	{
		// Copy data from incoming stream
		m_size = _sourceStream.size();
		m_data = new unsigned char[m_size];
		m_pos = m_data;
		m_end = m_data + _sourceStream.read( m_data, m_size );
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( DataStream* _sourceStream, bool _freeOnClose )
		: DataStream()
	{
		// Copy data from incoming stream
		m_size = _sourceStream->size();
		m_data = new unsigned char[m_size];
		m_pos = m_data;
		m_end = m_data + _sourceStream->read( m_data, m_size );
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( const Menge::String& _name, DataStream& _sourceStream, bool _freeOnClose)
		: DataStream( _name )
	{
		// Copy data from incoming stream
		m_size = _sourceStream.size();
		m_data = new unsigned char[m_size];
		m_pos = m_data;
		m_end = m_data + _sourceStream.read( m_data, m_size );
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( const Menge::String& _name, DataStream* _sourceStream, bool _freeOnClose )
		: DataStream( _name )
	{
		// Copy data from incoming stream
		m_size = _sourceStream->size();
		m_data = new unsigned char[m_size];
		m_pos = m_data;
		m_end = m_data + _sourceStream->read( m_data, m_size );
		m_freeOnClose = _freeOnClose;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( std::size_t _size, bool _freeOnClose )
		: DataStream()
	{
		m_size = _size;
		m_freeOnClose = _freeOnClose;
		m_data = new unsigned char[_size];
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::MemoryDataStream( const Menge::String& _name, std::size_t _size, bool _freeOnClose)
		: DataStream( _name )
	{
		m_size = _size;
		m_freeOnClose = _freeOnClose;
		m_data = new unsigned char[_size];
		m_pos = m_data;
		m_end = m_data + m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* MemoryDataStream::getPtr()
	{ 
		return m_data; 
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* MemoryDataStream::getCurrentPtr()
	{ 
		return m_pos; 
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryDataStream::~MemoryDataStream()
	{
		close();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MemoryDataStream::read( void* _buf, std::size_t _count )
	{
		std::size_t cnt = _count;
		// Read over end of memory?
		if ( m_pos + cnt > m_end )
			cnt = m_end - m_pos;
		if ( cnt == 0 )
			return 0;

		memcpy( _buf, m_pos, cnt );
		m_pos += cnt;
		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MemoryDataStream::readLine(char* _buf, std::size_t _maxCount, const Menge::String& _delim)
	{
		// Deal with both Unix & Windows LFs
		bool trimCR = false;
		if ( _delim.find_first_of('\n') != Menge::String::npos )
		{
			trimCR = true;
		}

		std::size_t pos = 0;

		// Make sure pos can never go past the end of the data 
		while ( pos < _maxCount && m_pos < m_end )
		{
			if ( _delim.find(*m_pos) != Menge::String::npos )
			{
				// Trim off trailing CR if this was a CR/LF entry
				if (trimCR && pos && _buf[pos-1] == '\r')
				{
					// terminate 1 character early
					--pos;
				}

				// Found terminator, skip and break out
				++m_pos;
				break;
			}

			_buf[pos++] = *m_pos++;
		}

		// terminate
		_buf[pos] = '\0';

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MemoryDataStream::skipLine( const Menge::String& _delim )
	{
		std::size_t pos = 0;

		// Make sure pos can never go past the end of the data 
		while ( m_pos < m_end )
		{
			++pos;
			if ( _delim.find(*m_pos++) != Menge::String::npos )
			{
				// Found terminator, break out
				break;
			}
		}

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::skip( long _count )
	{
		std::size_t newpos = (std::size_t)( ( m_pos - m_data ) + _count );
		assert( m_data + newpos <= m_end );        

		m_pos = m_data + newpos;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::seek( std::size_t _pos )
	{
		assert( m_data + _pos <= m_end );
		m_pos = m_data + _pos;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MemoryDataStream::tell() const
	{
		//m_data is start, m_pos is current location
		return m_pos - m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryDataStream::eof() const
	{
		return m_pos >= m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::close()
	{
		if ( m_freeOnClose && m_data )
		{
			delete [] m_data;
			m_data = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryDataStream::setFreeOnClose( bool _free )
	{ 
		m_freeOnClose = _free; 
	}
	//////////////////////////////////////////////////////////////////////////
	void* MemoryDataStream::getBuffer()
	{
		return getPtr();
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryDataStream::isMemory() const
	{
		return true;
	}
}