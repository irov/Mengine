#include "SoundBuffer.h"

SoundBufferInterface::SoundBufferInterface(void* _buffer, size_t _size) 
{
	mBits   = (unsigned char *)_buffer;
	mLength = _size;
	mPos    = 0;
	mFreq   = 0;
	mNumChannels   = 0;
	mBitsPerSample = 8;
}

SoundBufferInterface::~SoundBufferInterface()
{
}

void	SoundBufferInterface::setSize(int _rhs)
{
	mLength = _rhs;
}

void	SoundBufferInterface::setNumChannels(int _rhs)
{
	mNumChannels = _rhs;
}

void	SoundBufferInterface::setFrequency(int _rhs)
{
	mFreq = _rhs;
}

void	SoundBufferInterface::setBitsPerSample(int _rhs)
{
	mBitsPerSample = _rhs;
}

int		SoundBufferInterface::getNumChannels() const
{
	return mNumChannels;
}

int		SoundBufferInterface::getFrequency() const
{
	return mFreq; 
}

int		SoundBufferInterface::getBitsPerSample() const
{
	return mBitsPerSample; 
}

bool	SoundBufferInterface::atEnd() const
{
	return getPos() >= getSize();
}

int	SoundBufferInterface::getSize() const
{	
	return mLength;
}

int	SoundBufferInterface::getPos() const
{
	return mPos;
}

long SoundBufferInterface::getLong()
{
	if (mPos + 3 >= getSize())
	{
		assert(!"getLong error");
		return -1;
	};
	long 	v = *(long *) (mBits + mPos);
	mPos += 4;
	return v;
}

int	SoundBufferInterface::seekCur(int _delta)
{
	mPos += _delta;
	return (mPos > getSize()) ? getSize() : ((mPos < 0) ? 0 : mPos);
}

int	SoundBufferInterface::seekAbs(int _offs)
{
	mPos = _offs;
	return (mPos > getSize()) ? getSize() : ((mPos < 0) ? 0 : mPos);
}

int	SoundBufferInterface::getPtr(void * _ptr, int _len)
{
	if (mPos >= getSize())
	{
		return -1;
	};

	if (mPos + _len > getSize())
	{
		_len = mLength - mPos;
	};

	memcpy (_ptr, mBits + mPos, _len);

	mPos += _len;
	return _len;
};