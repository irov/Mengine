#include "SoundBuffer.h"

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "Vorbis/vorbisfile.h"

#include <memory.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
SoundBufferInterface::SoundBufferInterface(void* _buffer, size_t _size) 
: mBits((unsigned char *)_buffer)
, mLength(_size)
, mPos(0)
, mFreq(0)
, mNumChannels(0)
, mBitsPerSample(8)
{

}
//////////////////////////////////////////////////////////////////////////
SoundBufferInterface::~SoundBufferInterface()
{
}
//////////////////////////////////////////////////////////////////////////
void	SoundBufferInterface::setSize(size_t _rhs)
{
	mLength = _rhs;
}
//////////////////////////////////////////////////////////////////////////
void	SoundBufferInterface::setNumChannels(size_t _rhs)
{
	mNumChannels = _rhs;
}
//////////////////////////////////////////////////////////////////////////
void	SoundBufferInterface::setFrequency(size_t _rhs)
{
	mFreq = _rhs;
}
//////////////////////////////////////////////////////////////////////////
void	SoundBufferInterface::setBitsPerSample(size_t _rhs)
{
	mBitsPerSample = _rhs;
}
//////////////////////////////////////////////////////////////////////////
size_t		SoundBufferInterface::getNumChannels() const
{
	return mNumChannels;
}
//////////////////////////////////////////////////////////////////////////
size_t SoundBufferInterface::getFrequency() const
{
	return mFreq; 
}
//////////////////////////////////////////////////////////////////////////
size_t SoundBufferInterface::getBitsPerSample() const
{
	return mBitsPerSample; 
}
//////////////////////////////////////////////////////////////////////////
bool SoundBufferInterface::atEnd() const
{
	return getPos() >= getSize();
}
//////////////////////////////////////////////////////////////////////////
size_t	SoundBufferInterface::getSize() const
{	
	return mLength;
}
//////////////////////////////////////////////////////////////////////////
size_t	SoundBufferInterface::getPos() const
{
	return mPos;
}
//////////////////////////////////////////////////////////////////////////
long SoundBufferInterface::getLong()
{
	if (mPos + 3 >= mLength)
	{
		assert(!"getLong error");
		return -1;
	};
	long 	v = *(long *) (mBits + mPos);
	mPos += 4;
	return v;
}
//////////////////////////////////////////////////////////////////////////
size_t	SoundBufferInterface::seekCur(size_t _delta)
{
	mPos += _delta;
	return (mPos > mLength) ? mLength : ((mPos < 0) ? 0 : mPos);
}
//////////////////////////////////////////////////////////////////////////
size_t	SoundBufferInterface::seekAbs(size_t _offs)
{
	mPos = _offs;
	return (mPos > mLength) ? mLength : ((mPos < 0) ? 0 : mPos);
}
//////////////////////////////////////////////////////////////////////////
size_t	SoundBufferInterface::getPtr(void * _ptr, size_t _len)
{
	if (mPos >= mLength)
	{
		return -1;
	};

	if (mPos + _len > mLength)
	{
		_len = mLength - mPos;
	};

	memcpy (_ptr, mBits + mPos, _len);

	mPos += _len;
	return _len;
};