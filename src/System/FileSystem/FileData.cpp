#include "FileData.h"
#include <assert.h>

FileData::FileData(size_t _size) 
	: m_data(NULL)
	, m_pos(NULL)
	, m_endData(NULL)
	, m_size(_size)
{
	assert(m_size > 0);

	if(m_data)
	{
		delete[] m_data;
	}

	m_data = new char[m_size];
    m_pos = m_data;
	m_endData = m_data + m_size;
}

FileData::FileData(void *_data, size_t _size)
{
    m_data = reinterpret_cast< char* >(_data);
    m_endData = m_data + _size;
    m_pos = m_data;
    m_size = _size;
}

FileData::FileData(std::ifstream& _stream)
{
	_stream.seekg(0,std::ios::end); 
	std::streamsize filesize = _stream.tellg();
	_stream.seekg(0,std::ios::beg);
	m_size = filesize;
	m_data = new char[m_size];
    m_pos = m_data;
	m_endData = m_data + m_size;
	_stream.read(m_data,filesize);
}

size_t FileData::read(void* _buffer, size_t _elemsize, size_t _count)
{
    size_t cnt = _elemsize * _count;

	if ( m_endData < m_pos + cnt )
	{
        cnt = m_endData - m_pos;
	}

    if (cnt == 0)
	{
        return 0;
	}

    memcpy(_buffer, (const void*)m_pos, cnt);
    m_pos += cnt;

    return cnt;
}

size_t	FileData::read_chars(char* _buffer, size_t _count)
{
	return read(_buffer, sizeof(char), _count);
}

size_t	FileData::read_floats(float* _buffer, size_t _count)
{
	return read(_buffer, sizeof(float), _count);
}

size_t	FileData::read_ushorts( short* _buffer, size_t _count)
{
	return read(_buffer, sizeof(short), _count);
}

size_t	FileData::read_ints( int*_buffer, size_t _count)
{
	return read(_buffer, sizeof(int), _count);
}

size_t	FileData::read_longs( long* _buffer, size_t _count)
{
	return read(_buffer, sizeof(long), _count);
}

bool	FileData::eof() const
{
	return m_pos >= m_endData;
}

void	FileData::setPos(size_t _pos)
{
    if( _pos <= m_size )
	{
        m_pos = m_data + _pos;
	}
}

FileData::~FileData()
{
	if (m_data)
	{
		delete [] m_data;
		m_data = 0;
		m_size = 0;
	}
}

size_t		FileData::size() const 
{
	return m_size;
};

const char* FileData::getBuffer() const
{
	return m_data; 
};