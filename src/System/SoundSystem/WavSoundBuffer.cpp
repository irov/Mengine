#	include "WavSoundBuffer.h"

#	include <assert.h>

CALWavBuffer::CALWavBuffer(void * _buffer, size_t _size):SoundBufferInterface(_buffer, _size)
{
	long	magic	=  getLong();
	long	length	=  getLong();
	long	magic2	=  getLong();

	if ( magic != 0x46464952 || magic2 != 0x45564157 )
	{
		assert(!"CALWavBuffer error");
		return;
	}

	while ( getPos () <  getSize () )
	{
		magic  =  getLong ();
		length =  getLong ();

		if ( magic == 0x20746D66 )
		{
#pragma pack (push, 1)
			struct WavFmt
			{
				unsigned short encoding;
				unsigned short channels;
				unsigned int   freqeuncy;
				unsigned int   byterate;
				unsigned short blockAlign;
				unsigned short bps;
			} mFormat;
#pragma pack (pop)

			getPtr ( &mFormat, sizeof ( mFormat ) );
			if ( mFormat.encoding != 1 )
			{
				return;
			};
			setFrequency(mFormat.freqeuncy);
			setNumChannels(mFormat.channels);
			setBitsPerSample(mFormat.bps);
		}
		else
			if ( magic == 0x61746164 )
			{
				mDataOffs   =  getPos();
				mDataLength = length;
				break;
			}
			else
			{
				seekCur(length);
			};
	}

	if ( getNumChannels() < 1 || getFrequency() == 0 || mDataOffs == 0 || mDataLength == 0 )
	{
		assert(!"CALWavBuffer error");
		return;
	}
};

CALWavBuffer::~CALWavBuffer(){};

size_t	CALWavBuffer::read(unsigned char * _buffer, size_t _size)
{
	size_t	bytesLeft = mDataLength + mDataOffs - getPos();
	return  getPtr(_buffer, ( _size < 0 || _size > bytesLeft ) ? bytesLeft : _size);
}

bool	CALWavBuffer::seek(float _time)
{
	seekAbs(mDataOffs + (int)(_time * getNumChannels() * getFrequency() * getBitsPerSample()/2));	
	return true;
}

size_t CALWavBuffer::getDataSoundSize() const
{
	return mDataLength;	
};