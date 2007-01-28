#	include "WavSoundBuffer.h"

#	include <assert.h>

#	include "includes/OpenAL/al.h"
#	include "includes/OpenAL/alut.h"

#pragma pack (push, 1)
struct WavFmt 
{
	unsigned short encoding;
	unsigned short channels; 
	unsigned int freqeuncy; 
	unsigned int byterate; 
	unsigned short blockAlign; 
	unsigned short bps;
};
#pragma pack (pop)


CALWavBuffer::CALWavBuffer(void * _buffer, size_t _size)
: SoundBufferInterface(_buffer, _size)
{
/*	ALenum		format;
	ALsizei		size;
	ALvoid    * data;
	ALsizei 	freq;
	ALboolean	loop;

	alutLoadWAVMemory((ALbyte *)_buffer, &format, &data, &size, &freq, &loop );

	setFrequency(freq);
	setNumChannels(1);
	setBitsPerSample(size);
*/
	enum
	{
		RIFF = 0x46464952,
		WAVE = 0x45564157,
		FMT  = 0x20746D66,
		DATA = 0x61746164,
	};

	long magic = getLong();
	long length = getLong();
	long magic2 = getLong();

	if (magic != RIFF || magic2 != WAVE)
	{
		assert(!"CALWavBuffer error");
		return;
	}

	for (;getPos() < getLength();)
	{
		magic = getLong();
		length =  getLong();

		if (magic == FMT)
		{
			WavFmt wave_format;

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
			if (magic == DATA)
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

ALsizei	CALWavBuffer::read(unsigned char * _buffer, size_t _size)
{
	size_t	bytesLeft = m_dataLength - (getPos() - m_dataOffset);

	if (_size < 0 || _size > bytesLeft)
	{
		_size = bytesLeft;
	}

	return  (ALsizei)getPtr(_buffer, _size);
}

bool	CALWavBuffer::seek(float _time)
{
	seekAbs(m_dataOffset + (int)(_time * getNumChannels() * getFrequency() * getBitsPerSample()/2));	
	return true;
}

size_t	CALWavBuffer::getDataSoundSize() const
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