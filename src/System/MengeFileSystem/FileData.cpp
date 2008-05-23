
#	include "FileData.h"

#	include <assert.h>
#	include <fstream>

//////////////////////////////////////////////////////////////////////////
DataStream::DataStream()
: m_size( 0 ) 
{
}
//////////////////////////////////////////////////////////////////////////
DataStream::DataStream( const std::string& _name )
: m_name( _name )
, m_size( 0 ) 
{
}
//////////////////////////////////////////////////////////////////////////
const std::string& DataStream::getName() const
{
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
template <typename T> DataStream& DataStream::operator >>( T& _val )
{
	read(static_cast<void*>(&_val), sizeof(T));
	return *this;
}
//////////////////////////////////////////////////////////////////////////
std::string DataStream::getLine( bool _trimAfter )
{
	char tmpBuf[MENGE_STREAM_TEMP_SIZE];
	std::string retString;
	std::size_t readCount;
	// Keep looping while not hitting delimiter
	while ( ( readCount = read( tmpBuf, MENGE_STREAM_TEMP_SIZE - 1 ) ) != 0 )
	{
		// Terminate string
		tmpBuf[readCount] = '\0';

		char* p = strchr( tmpBuf, '\n' );
		if ( p != 0 )
		{
			// Reposition backwards
			skip( (long)( p + 1 - tmpBuf - readCount ) );
			*p = '\0';
		}

		retString += tmpBuf;

		if ( p != 0 )
		{
			// Trim off trailing CR if this was a CR/LF entry
			if ( retString.length() && retString[retString.length()-1] == '\r' )
			{
				retString.erase( retString.length() - 1, 1 );
			}

			// Found terminator, break out
			break;
		}
	}

	if ( _trimAfter )
	{
		static const std::string delims = " \t\r";
		retString.erase( retString.find_last_not_of( delims ) + 1 ); // trim right
		retString.erase( 0, retString.find_first_not_of( delims ) ); // trim left
	}

	return retString;
}
//////////////////////////////////////////////////////////////////////////
std::size_t DataStream::readLine( char* _buf, std::size_t _maxCount, const std::string& _delim )
{
	// Deal with both Unix & Windows LFs
	bool trimCR = false;
	if ( _delim.find_first_of('\n') != std::string::npos )
	{
		trimCR = true;
	}

	char tmpBuf[MENGE_STREAM_TEMP_SIZE];
	std::size_t chunkSize = (std::min)( _maxCount, (std::size_t)MENGE_STREAM_TEMP_SIZE - 1 );
	std::size_t totalCount = 0;
	std::size_t readCount; 
	while ( chunkSize && ( readCount = read( tmpBuf, chunkSize ) ) )
	{
		// Terminate
		tmpBuf[readCount] = '\0';

		// Find first delimiter
		std::size_t pos = strcspn( tmpBuf, _delim.c_str() );

		if( pos < readCount )
		{
			// Found terminator, reposition backwards
			skip( (long)( pos + 1 - readCount ) );
		}

		// Are we genuinely copying?
		if( _buf )
		{
			memcpy( _buf + totalCount, tmpBuf, pos );
		}
		totalCount += pos;

		if( pos < readCount )
		{
			// Trim off trailing CR if this was a CR/LF entry
			if ( trimCR && totalCount && _buf[totalCount-1] == '\r')
			{
				--totalCount;
			}

			// Found terminator, break out
			break;
		}

		// Adjust chunkSize for next time
		chunkSize = (std::min)( _maxCount - totalCount, (std::size_t)MENGE_STREAM_TEMP_SIZE - 1 );
	}

	// Terminate
	_buf[totalCount] = '\0';

	return totalCount;
}
//////////////////////////////////////////////////////////////////////////
std::size_t DataStream::skipLine( const std::string& _delim )
{
	char tmpBuf[MENGE_STREAM_TEMP_SIZE];
	std::size_t total = 0;
	std::size_t readCount;
	// Keep looping while not hitting delimiter
	while ( ( readCount = read( tmpBuf, MENGE_STREAM_TEMP_SIZE - 1 ) ) != 0 )
	{
		// Terminate string
		tmpBuf[readCount] = '\0';

		// Find first delimiter
		std::size_t pos = strcspn( tmpBuf, _delim.c_str() );

		if( pos < readCount )
		{
			// Found terminator, reposition backwards
			skip( (long)( pos + 1 - readCount ) );

			total += pos + 1;

			// break out
			break;
		}

		total += readCount;
	}

	return total;
}
//////////////////////////////////////////////////////////////////////////
std::string DataStream::getAsString()
{
	// Read the entire buffer
	char* pBuf = new char[m_size+1];
	// Ensure read from begin of stream
	seek(0);
	read( pBuf, m_size );
	pBuf[m_size] = '\0';
	std::string str;
	str.insert( 0, pBuf, m_size );
	delete [] pBuf;
	return str;
}
//////////////////////////////////////////////////////////////////////////
std::size_t DataStream::size() const
{
	return m_size;
}
//////////////////////////////////////////////////////////////////////////
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
MemoryDataStream::MemoryDataStream( const std::string& _name, void* _pMem, std::size_t _size, 
	bool _freeOnClose)
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
MemoryDataStream::MemoryDataStream( const std::string& _name, DataStream& _sourceStream, 
	bool _freeOnClose)
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
MemoryDataStream::MemoryDataStream( const std::string& _name, DataStream* _sourceStream, 
	bool _freeOnClose )
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
MemoryDataStream::MemoryDataStream( const std::string& _name, std::size_t _size, 
	bool _freeOnClose)
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
std::size_t MemoryDataStream::readLine(char* _buf, std::size_t _maxCount, 
										const std::string& _delim)
{
	// Deal with both Unix & Windows LFs
	bool trimCR = false;
	if ( _delim.find_first_of('\n') != std::string::npos )
	{
		trimCR = true;
	}

	std::size_t pos = 0;

	// Make sure pos can never go past the end of the data 
	while ( pos < _maxCount && m_pos < m_end )
	{
		if ( _delim.find(*m_pos) != std::string::npos )
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
std::size_t MemoryDataStream::skipLine( const std::string& _delim )
{
	std::size_t pos = 0;

	// Make sure pos can never go past the end of the data 
	while ( m_pos < m_end )
	{
		++pos;
		if ( _delim.find(*m_pos++) != std::string::npos )
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
//////////////////////////////////////////////////////////////////////////
FileStreamDataStream::FileStreamDataStream( std::ifstream* _s, bool _freeOnClose )
: DataStream()
, m_stream( _s )
, m_freeOnClose( _freeOnClose )
{
	// calculate the size
	m_stream->seekg( 0, std::ios_base::end );
	m_size = m_stream->tellg();
	m_stream->seekg( 0, std::ios_base::beg );

}
//////////////////////////////////////////////////////////////////////////
FileStreamDataStream::FileStreamDataStream( const std::string& _name, std::ifstream* _s, bool _freeOnClose )
: DataStream( _name )
, m_stream( _s )
, m_freeOnClose( _freeOnClose )
{
	// calculate the size
	m_stream->seekg( 0, std::ios_base::end );
	m_size = m_stream->tellg();
	m_stream->seekg( 0, std::ios_base::beg );
}
//////////////////////////////////////////////////////////////////////////
FileStreamDataStream::FileStreamDataStream( const std::string& _name, 
	std::ifstream* _s, std::size_t _size, bool _freeOnClose )
: DataStream( _name )
, m_stream( _s )
, m_freeOnClose( _freeOnClose )
{
	m_size = _size;
}
//////////////////////////////////////////////////////////////////////////
FileStreamDataStream::~FileStreamDataStream()
{
	close();
}
//////////////////////////////////////////////////////////////////////////
std::size_t FileStreamDataStream::read( void* _buf, std::size_t _count )
{
	m_stream->read( static_cast<char*>(_buf), static_cast<std::streamsize>(_count) );
	return m_stream->gcount();
}
//////////////////////////////////////////////////////////////////////////
std::size_t FileStreamDataStream::readLine( char* _buf, std::size_t _maxCount, 
											const std::string& _delim )
{
	assert( !_delim.empty() &&  "FileStreamDataStream::readLine -> No delimiter provided" );

	/*if (delim.size() > 1)
	{
		LogManager::getSingleton().logMessage(
			"WARNING: FileStreamDataStream::readLine - using only first delimeter");
	}*/

	// Deal with both Unix & Windows LFs
	bool trimCR = false;
	if ( _delim.at(0) == '\n' ) 
	{
		trimCR = true;
	}
	// _maxCount + 1 since count excludes terminator in getline
	m_stream->getline( _buf, static_cast<std::streamsize>( _maxCount + 1 ), _delim.at( 0 ) );
	std::size_t ret = m_stream->gcount();
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
	if ( trimCR && _buf[ret-1] == '\r' )
	{
		--ret;
		_buf[ret] = '\0';
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////
void FileStreamDataStream::skip( long _count )
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
	m_stream->seekg( static_cast<std::ifstream::pos_type>( _count ), std::ios::cur );
}
//////////////////////////////////////////////////////////////////////////
void FileStreamDataStream::seek( std::size_t _pos )
{
	m_stream->clear(); //Clear fail status in case eof was set
	m_stream->seekg( static_cast<std::streamoff>(_pos), std::ios::beg );
}
//////////////////////////////////////////////////////////////////////////
std::size_t FileStreamDataStream::tell() const
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
}
//////////////////////////////////////////////////////////////////////////
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
FileHandleDataStream::FileHandleDataStream( const std::string& _name, FILE* _handle )
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
std::size_t FileHandleDataStream::read( void* _buf, std::size_t _count )
{
	return fread( _buf, 1, _count, m_fileHandle );
}
//////////////////////////////////////////////////////////////////////////
void FileHandleDataStream::skip( long _count )
{
	fseek( m_fileHandle, _count, SEEK_CUR);
}
//////////////////////////////////////////////////////////////////////////
void FileHandleDataStream::seek( std::size_t _pos )
{
	fseek(m_fileHandle, static_cast<long>(_pos), SEEK_SET);
}
//////////////////////////////////////////////////////////////////////////
std::size_t FileHandleDataStream::tell() const
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
//////////////////////////////////////////////////////////////////////////
MengeFileData::MengeFileData( DataStream* _data )
: m_data( _data )
{
}
//////////////////////////////////////////////////////////////////////////
MengeFileData::~MengeFileData()
{
	m_data->close();
}
//////////////////////////////////////////////////////////////////////////
std::size_t MengeFileData::read( void* _buffer, std::size_t _count )
{
	return m_data->read( _buffer, _count );
}
//////////////////////////////////////////////////////////////////////////
bool MengeFileData::eof() const
{
	return m_data->eof();
}
//////////////////////////////////////////////////////////////////////////
std::size_t MengeFileData::size() const
{
	return m_data->size();
}
//////////////////////////////////////////////////////////////////////////
void MengeFileData::seek( std::size_t _pos )
{
	m_data->seek( _pos );
}
//////////////////////////////////////////////////////////////////////////
std::size_t MengeFileData::tell() const
{
	return m_data->tell();
}
//////////////////////////////////////////////////////////////////////////
std::string MengeFileData::getLine( bool _trimAfter ) const
{
	return m_data->getLine( _trimAfter );
}
//////////////////////////////////////////////////////////////////////////
std::size_t MengeFileData::skipLine( const std::string & _delim )
{
	return m_data->skipLine( _delim );
}
//////////////////////////////////////////////////////////////////////////