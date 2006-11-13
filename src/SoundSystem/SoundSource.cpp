#	include "SoundSource.h"

#	include "SoundBuffer.h"

#	include <assert.h>

#	include "includes/OpenAL/al.h"

OpenALSoundSource::OpenALSoundSource( 
									 SoundNodeListenerInterface* _plr,
									 SoundBufferInterface* _data,
									 bool _streaming) 
:	mLooping(true), 
	mHeadMode(false), 
	mVolume(1.0f), 
	mMaxDistance(100)
{
	mPlayer = _plr;
	mBufferWithData = _data;

	if (mBufferWithData == 0)
	{
		assert(!"OpenALSoundSource error");
		return;
	}

	mIsStream  = _streaming;

	if (mBufferWithData->getNumChannels() == 2)		
	{
		if ( mBufferWithData -> getBitsPerSample() == 16)
		{
			mFormat = AL_FORMAT_STEREO16;
		}
		else
		{
			mFormat = AL_FORMAT_STEREO8;
		}
	}
	else
	{
		if (mBufferWithData -> getBitsPerSample() == 16)
		{
			mFormat = AL_FORMAT_MONO16;
		}
		else
		{
			mFormat = AL_FORMAT_MONO8;
		}
	}
	
	if (mIsStream)
	{
		mDataSoundBuffer = new unsigned char [BUFFER_SIZE * 2];
		mCurrentBuffer = 0;
		alGenBuffers(2, mBuffer);
		alGenSources(1, &mSource);
	}
	else
	{
		mDataSoundBuffer = new unsigned char[mBufferWithData->getDataSoundSize()];
		int size = mBufferWithData->read(mDataSoundBuffer);
		alGenBuffers(1, mBuffer);
		alBufferData(mBuffer[0], mFormat, mDataSoundBuffer, size, mBufferWithData->getFrequency());
		alGenSources(1,&mSource);
		alSourcei(mSource, AL_BUFFER, mBuffer[0]);
		delete mDataSoundBuffer;
	}

	mPosition[0] = 0;
	mPosition[1] = 0;
	mPosition[2] = 0;
	float m_ov[3] = {};
	alSourcef(mSource, AL_GAIN, 1.0f);
	alSourcef(mSource, AL_PITCH, 1.0f);
	alSourcei(mSource, AL_LOOPING, mLooping);
	alSourcefv(mSource, AL_POSITION,  mPosition);
	alSourcefv(mSource, AL_VELOCITY,  m_ov);
	alSourcei(mSource, AL_SOURCE_RELATIVE, mHeadMode);
	mPlayer = 0;
}

OpenALSoundSource::~OpenALSoundSource()
{
	stop();
	delete mBufferWithData;

	if (mIsStream)
	{
		delete mDataSoundBuffer;
	}

	alDeleteSources(1, &mSource);

	if (mIsStream)
	{
		alDeleteBuffers(2,mBuffer);
	}
	else
	{
		alDeleteBuffers(1,mBuffer);
	}
}

void	OpenALSoundSource::play()
{
	ALint 	state;
	int		size;
	alGetSourcei (mSource, AL_SOURCE_STATE, &state);

	if (state == AL_PLAYING)
	{
		return;
	}

	if (state != AL_PAUSED && mIsStream)
	{
		size = mBufferWithData->read(mDataSoundBuffer, BUFFER_SIZE);
		alBufferData(mBuffer[0], mFormat, mDataSoundBuffer, size, mBufferWithData->getFrequency());
		size = mBufferWithData->read ( mDataSoundBuffer, BUFFER_SIZE );
		alBufferData(mBuffer[1], mFormat, mDataSoundBuffer, size, mBufferWithData->getFrequency());
		alSourceQueueBuffers(mSource, 2, mBuffer);
	}
	alSourcePlay (mSource);
}

void	OpenALSoundSource::pause()
{
	alSourcePause(mSource);
}

void	OpenALSoundSource::stop()
{
	alSourceStop(mSource);
	mBufferWithData->seek(0);

	if (mIsStream)
	{
		ALint queued;
		alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);
		if (queued > 0)
		{
			alSourceUnqueueBuffers(mSource, 2, mBuffer);
		}
		mCurrentBuffer = 0;
		if(mPlayer)
		{
			mPlayer->listenStoped( this );
		}
	}
}

bool	 OpenALSoundSource::updateSoundBuffer()
{
	ALint state;
	alGetSourcei(mSource, AL_SOURCE_STATE, &state);
	
	if (state == AL_STOPPED && mIsStream)
	{
		return false;
	}

	if (state == AL_PLAYING && mBufferWithData->atEnd() && getLooped())
	{
		if(mPlayer)
		{
			mPlayer->listenRecycled( this );
		}
	}

	if (mIsStream)
	{
		ALint processed;
		alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

		if (processed == 0)
		{
			return true;
		}

		if (processed == 1)
		{
			alSourceUnqueueBuffers(mSource, 1, &mBuffer[mCurrentBuffer]);
			int size = mBufferWithData->read (mDataSoundBuffer,BUFFER_SIZE);
			if (size > 0 || (size == 0 && mLooping))
			{
				alBufferData(mBuffer[mCurrentBuffer], mFormat, mDataSoundBuffer, size, mBufferWithData->getFrequency());
				alSourceQueueBuffers(mSource, 1, &mBuffer[mCurrentBuffer]);
				if (size < BUFFER_SIZE && mLooping)
				{
					mBufferWithData->seek(0);
				};
			}
			else
			{
				int queued;

				alGetSourcei(mSource,AL_BUFFERS_QUEUED,&queued);

				if ( queued == 0 )
				{
					mBufferWithData -> seek(0);
				}
			}
			mCurrentBuffer = 1 - mCurrentBuffer;
		}
		else
		if (processed == 2)
		{
			alSourceUnqueueBuffers(mSource, 2, mBuffer);
			mCurrentBuffer = 0;
			play();
		}
	}
	return true;
}

void	OpenALSoundSource::setLooped(bool _flag)	
{
	mLooping = _flag; 
	alSourcei(mSource, AL_LOOPING, mLooping);
}

void	OpenALSoundSource::setVolume(float _value)
{
	mVolume = _value;
	alSourcef(mSource, AL_GAIN, mVolume);
}

void	OpenALSoundSource::setPosition(const float* _position)
{
	mPosition[0] = _position[0];
	mPosition[1] = _position[1];
	mPosition[2] = _position[2];
	alSourcefv(mSource, AL_POSITION, mPosition);
}

void		OpenALSoundSource::setDistance(float _dist)	
{
	mMaxDistance = _dist;
	alSourcef(mSource, AL_MAX_DISTANCE, mMaxDistance);
}

void		OpenALSoundSource::setHeadMode(bool _flag) 
{
	mHeadMode = _flag;
	alSourcei(mSource, AL_SOURCE_RELATIVE, mHeadMode);
}

const float*	OpenALSoundSource::getPosition() const
{
	return	mPosition;
}

float			OpenALSoundSource::getDistance() const
{
	return	mMaxDistance;
}

float			OpenALSoundSource::getVolume() const
{
	return	mVolume;	
}

bool			OpenALSoundSource::getHeadMode() const	
{
	return	mHeadMode;	
}

bool			OpenALSoundSource::getLooped() const
{
	return	mLooping;	
}