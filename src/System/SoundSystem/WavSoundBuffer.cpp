#	include "WavSoundBuffer.h"

#	include <assert.h>

CALWavBuffer::CALWavBuffer(void * _buffer, size_t _size)
: SoundBufferInterface(_buffer, _size)
{
	long magic = getLong();
	long length = getLong();
	long magic2 = getLong();

	if (magic != 0x46464952 || magic2 != 0x45564157)
	{
		assert(!"CALWavBuffer error");
		return;
	}

	while (getPos() < getSize())
	{
		magic = getLong();
		length = getLong();

		if (magic == 0x20746D66)
		{
			#pragma pack (push, 1)
			struct WavFmt  { unsigned short encoding; unsigned short channels; unsigned int freqeuncy; unsigned int byterate; unsigned short blockAlign; unsigned short bps;  } wave_format;
			#pragma pack (pop)

			getPtr(&wave_format, sizeof(wave_format));

			if (wave_format.encoding != 1)
			{
				assert(!"Error encoding!");
				return;
			}

			setFrequency(wave_format.freqeuncy);
			setNumChannels(wave_format.channels);
			setBitsPerSample(wave_format.bps);
		}
		else
		{
			if (magic == 0x61746164)
			{
				m_dataOffset = getPos();
				m_dataLength = length;
				break;
			}
			else
			{
				seekCur(length);
			}
		}
	}

	if (getNumChannels() < 1 || getFrequency() == 0 || m_dataOffset == 0 || m_dataLength == 0)
	{
		assert(!"CALWavBuffer error");
		return;
	}
};

CALWavBuffer::~CALWavBuffer(){};

int		CALWavBuffer::read(unsigned char * _buffer, int _size)
{
	int	bytesLeft = m_dataLength - (getPos() - m_dataOffset);
	return  getPtr(_buffer, (_size < 0 || _size > bytesLeft) ? bytesLeft : _size);
}

bool	CALWavBuffer::seek(float _time)
{
	seekAbs(m_dataOffset + (int)(_time * getNumChannels() * getFrequency() * getBitsPerSample()/2));	
	return true;
}

int		CALWavBuffer::getDataSoundSize() const
{
	return m_dataLength;	
};

double	CALWavBuffer::getTotalTime()
{
	double byte_per_sample = getBitsPerSample() * getNumChannels() / 8;
	assert(byte_per_sample>=0);

	double freq = getFrequency();
	assert(freq>=0);

	return m_dataLength / byte_per_sample / freq;
}