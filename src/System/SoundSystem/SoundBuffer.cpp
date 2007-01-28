#include "SoundBuffer.h"

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "Vorbis/vorbisfile.h"

#include <memory.h>
#include <assert.h>


SoundBufferInterface::SoundBufferInterface(void* _buffer, size_t _size) 
{
	m_bits   = (unsigned char *)_buffer;
	m_length = _size;
	m_pos    = 0;
	m_freq   = 0;
	m_numChannels   = 0;
	m_bitsPerSample = 8;
}

SoundBufferInterface::~SoundBufferInterface()
{}

void	SoundBufferInterface::setSize(size_t _rhs)
{
	m_length = _rhs;
}

void	SoundBufferInterface::setNumChannels(size_t _rhs)
{
	m_numChannels = _rhs;
}

void	SoundBufferInterface::setFrequency(ALsizei _rhs)
{
	m_freq = _rhs;
}

void	SoundBufferInterface::setBitsPerSample(int _rhs)
{
	m_bitsPerSample = _rhs;
}

size_t		SoundBufferInterface::getNumChannels() const
{
	return m_numChannels;
}

ALsizei		SoundBufferInterface::getFrequency() const
{
	return m_freq; 
}

int		SoundBufferInterface::getBitsPerSample() const
{
	return m_bitsPerSample; 
}

bool	SoundBufferInterface::atEnd() const
{
	return getPos() >= getLength();
}

size_t SoundBufferInterface::getLength() const
{	
	return m_length;
}

size_t	SoundBufferInterface::getPos() const
{
	return m_pos;
}

long SoundBufferInterface::getLong()
{
	if (m_pos + 3 >= m_length)
	{
		assert(!"getLong error");
		return -1;
	};
	long 	v = *(long *) (m_bits + m_pos);
	m_pos += 4;
	return v;
}

size_t	SoundBufferInterface::seekCur(int _delta)
{

	m_pos += _delta;

	if (m_pos > m_length)
		m_pos = m_length;

	if (m_pos < 0)
		m_pos = 0;

//	m_pos += _delta;
	return m_pos;
}

size_t	SoundBufferInterface::seekAbs(size_t _offs)
{
	m_pos = _offs;
	return (m_pos > getLength()) ? getLength() : ((m_pos < 0) ? 0 : m_pos);
}

size_t	SoundBufferInterface::getPtr(void * _ptr, size_t _len)
{
	if (m_pos >= getLength())
	{
		return -1;
	};

	if (m_pos + _len > getLength())
	{
		_len = m_length - m_pos;
	};

	memcpy (_ptr, m_bits + m_pos, _len);

	m_pos += _len;
	return _len;
};