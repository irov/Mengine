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

void	SoundBufferInterface::setSize(int _rhs)
{
	m_length = _rhs;
}

void	SoundBufferInterface::setNumChannels(int _rhs)
{
	m_numChannels = _rhs;
}

void	SoundBufferInterface::setFrequency(int _rhs)
{
	m_freq = _rhs;
}

void	SoundBufferInterface::setBitsPerSample(int _rhs)
{
	m_bitsPerSample = _rhs;
}

int		SoundBufferInterface::getNumChannels() const
{
	return m_numChannels;
}

int		SoundBufferInterface::getFrequency() const
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

int SoundBufferInterface::getLength() const
{	
	return m_length;
}

int	SoundBufferInterface::getPos() const
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

int	SoundBufferInterface::seekCur(int _delta)
{

	m_pos += _delta;

	if (m_pos > m_length)
		m_pos = m_length;

	if (m_pos < 0)
		m_pos = 0;

//	m_pos += _delta;
	return m_pos;
}

int	SoundBufferInterface::seekAbs(int _offs)
{
	m_pos = _offs;
	return (m_pos > getLength()) ? getLength() : ((m_pos < 0) ? 0 : m_pos);
}

int	SoundBufferInterface::getPtr(void * _ptr, int _len)
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