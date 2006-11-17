#include "FileData.h"
#include <assert.h>

FileData::FileData(size_t _size) 
	: mData(NULL),
	mPos(NULL),
	mEndData(NULL),
	mSize(_size)
{
	assert(mSize > 0);

	if(mData)
	{
		delete[] mData;
	}

	mData = new unsigned char[mSize];
    mPos = mData;
	mEndData = mData + mSize;
}

FileData::FileData(void *_data, size_t _size)
{
    mData = reinterpret_cast< unsigned char* >(_data);
    mEndData = mData + _size;
    mPos = mData;
    mSize = _size;
}

FileData::FileData(std::ifstream& _stream)
{
	_stream.seekg(0,std::ios::end); 
	std::streamsize filesize = _stream.tellg();
	_stream.seekg(0,std::ios::beg);
	mSize = filesize;
	mData = new unsigned char[mSize];
    mPos = mData;
	mEndData = mData + mSize;
	_stream.read((char*)getBuffer(),filesize);
}

size_t FileData::read(void* _buffer, size_t _elemsize, size_t _count)
{
    size_t cnt = _elemsize * _count;

	if ( mEndData - mPos < cnt )
	{
        cnt = mEndData - mPos;
	}

    if (cnt == 0)
	{
        return 0;
	}

    memcpy(_buffer, (const void*)mPos, cnt);
    mPos += cnt;

    return cnt;
}

size_t	FileData::read_chars(unsigned char* _buffer, size_t _count)
{
	return read(_buffer, sizeof(char), _count);
}

size_t	FileData::read_floats(float* _buffer, size_t _count)
{
	return read(_buffer, sizeof(float), _count);
}

size_t	FileData::read_ushorts(unsigned short* _buffer, size_t _count)
{
	return read(_buffer, sizeof(unsigned short), _count);
}

size_t	FileData::read_ints(unsigned int*_buffer, size_t _count)
{
	return read(_buffer, sizeof(unsigned int), _count);
}

size_t	FileData::read_longs(unsigned long* _buffer, size_t _count)
{
	return read(_buffer, sizeof(unsigned long), _count);
}

bool	FileData::eof() const
{
	return mPos >= mEndData;
}

void	FileData::setPos(size_t _pos)
{
    if( _pos <= mSize )
	{
        mPos = mData + _pos;
	}
}

FileData::~FileData()
{
	if (mData)
	{
		delete [] mData;
		mData = 0;
		mSize = 0;
	}
}

size_t		FileData::size() const 
{
	return mSize;
};

const unsigned char* FileData::getBuffer() const
{
	return mData; 
};