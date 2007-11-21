
#include "stdafx.h"

#include "AL/ALut.h"
#include "Vorbis/Vorbisfile.h"

#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

#include <stdlib.h>

UINT DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, UINT ulBufferSize, UINT ulChannels);
void Swap(short &s1, short &s2);
UINT GetSampleSize(ALenum format);


ALSoundSystem::ALSoundSystem() :
mSoundVelocity(343.0f),
mDopplerFactor(1.0f),
mDistanceModel(None),
m_sourceNum(0)
{
	char *initString = 0L;//"DirectSound", "DirectSound3D", ;
	mDevice = alcOpenDevice((const ALCchar *)initString);

    if(!mDevice)
	{
		// TODO: error handling
	}

    int attributes[7],i=0;
    attributes[0]=0;

    mContext = alcCreateContext(mDevice, attributes);
    if(!mContext || alcGetError(mDevice) != ALC_NO_ERROR) 
	{
      if(mContext)
	      alcDestroyContext(mContext);
      alcCloseDevice(mDevice);

    } 
    alcMakeContextCurrent(mContext);
 
	// get all available sources
	for(;m_sourceNum < MAX_SOURCE_NUM; m_sourceNum++)
	{
		alGenSources(1, &m_sources[m_sourceNum]);	
		if(alGetError())
			break;
	}
	/*ALuint t;
	for(int i=0; i < 30; i++)
	alGenSources(1, &t);
	int ms[1] = { -1 };
	alcGetIntegerv(mDevice, ALC_STEREO_SOURCES, 1, ms);
	printf("%d\n", ms[1]);*/
	// Check for EAX 2.0 support
  /*  unsigned char szFnName[256];
    ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
    if (g_bEAX == AL_TRUE)
    {
      sprintf((char*)szFnName, "EAXSet");
      ALvoid *eaxSet = alGetProcAddress((const ALchar*)szFnName);
      if (eaxSet == NULL) g_bEAX = AL_FALSE;
    }
    if (g_bEAX == AL_TRUE)
    {
      sprintf((char*)szFnName,"EAXGet");
      ALvoid *eaxGet = alGetProcAddress((const ALchar*)szFnName);
      if (eaxGet == NULL) g_bEAX = AL_FALSE;
    }
    if (g_bEAX == AL_TRUE)
	{
		WF_INFO_LOG("Using OpenAL EAX2.0 extension");
      //std::cerr << "Using OpenAL EAX2.0 extension" << std::endl;
	}
    else
	{
		WF_INFO_LOG("No OpenAL EAX2.0 extensions available");
      //std::cerr << "No OpenAL EAX2.0 extensions available" << std::endl;
	}
  }*/

	// Environment settings 
	alSpeedOfSound(mSoundVelocity);
	//alDopplerVelocity(mSoundVelocity);
	alDopplerFactor(mDopplerFactor);
	setDistanceModel(mDistanceModel);
}

ALSoundSystem::~ALSoundSystem()
{
	alDeleteSources(m_sourceNum, m_sources);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(mContext);
	alcCloseDevice(mDevice);
}

void    ALSoundSystem::setListenerOrient( float * _position, float * _front, float * _top)
{
	memcpy( mListenerPosition, _position, sizeof(float)*3 );
	memcpy( mListenerOrientation, _front, sizeof(float)*3 );
	memcpy( mListenerOrientation+3, _top, sizeof(float)*3 );

	alListenerfv(AL_POSITION, mListenerPosition);
	//alListenerfv(AL_VELOCITY, velocity_);
	alListenerfv(AL_ORIENTATION, mListenerOrientation);
}

SoundSourceInterface*   ALSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface * _listener )
{
	ALSoundSource* source = new ALSoundSource(this);
	source->setSoundBuffer( static_cast<ALSoundBuffer*>(_sample) );
	source->setAmbient(_isHeadMode);
	return source;
}

SoundBufferInterface *  ALSoundSystem::createSoundBufferFromFile( const char * _filename, bool _isStream )
{
	ALSoundBuffer* buffer = NULL;

	if(_isStream)
	{
		buffer = new ALSoundBufferStream(_filename);
	}
	else
	{
		buffer = new ALSoundBuffer();

		ALsizei size,freq;
		ALenum format;
		ALvoid *data = NULL;
		ALboolean loop;

		if( !strcmp( _filename + (strlen(_filename) - 4), ".wav" ) )
		{
			alutLoadWAVFile((ALbyte*)_filename, &format, &data, &size, &freq, &loop);
			
			buffer->setLenghtMs(size * 1000 / (freq * GetSampleSize(format) ));
		}
		else if( !strcmp( _filename + (strlen(_filename) - 4), ".ogg" ) )
		{
			OggVorbis_File oggfile;
			FILE* filehandle = fopen(_filename, "rb");
			if(ov_open(filehandle, &oggfile, NULL, 0) >= 0) 
			{
				vorbis_info *ogginfo = ov_info(&oggfile,-1);
				freq = ogginfo->rate; 
				if(ogginfo->channels == 1)
					format = AL_FORMAT_MONO16;
				else if(ogginfo->channels == 2)
					format = AL_FORMAT_STEREO16;
				else if(ogginfo->channels == 4)
					format = alGetEnumValue("AL_FORMAT_QUAD16");
				else if(ogginfo->channels == 6)
					format = alGetEnumValue("AL_FORMAT_51CHN16");

				buffer->setLenghtMs(ov_time_total(&oggfile, -1) * 1000);

				size = ov_pcm_total(&oggfile, -1) * GetSampleSize(format);

				data = malloc(size);
				DecodeOggVorbis(&oggfile, (char*)data, size, ogginfo->channels);

			} 
			else 
			{
				fclose(filehandle);
			}
			ov_clear(&oggfile);
		}

		if(data) 
		{
			//buffer->setLenghtMs(size * 1000 / (freq * GetSampleSize(format)));
			alBufferData( buffer->getBufferName(), format, data, size, freq );
			/*if( (error = alGetError()) != AL_FALSE )
			{	
			}*/
			free(data);
		} 
	}
	return buffer;
}
SoundBufferInterface *  ALSoundSystem::createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem )
{
	ALSoundBuffer* buffer = new ALSoundBuffer();
	ALsizei size,freq;
	ALenum format;
	ALvoid *data = NULL;
	ALboolean loop;

	//if( !strcmp( _filename + (strlen(_filename) - 4), ".wav" ) )
	{
		alutLoadWAVMemory((ALbyte*)_buffer, &format, &data, &size, &freq, &loop);
	}

	if(data) 
	{
		buffer->setLenghtMs(size * 1000 / (freq * GetSampleSize(format)));
		alBufferData( buffer->getBufferName(), format, data, size, freq );
	/*	if( (error = alGetError()) != AL_FALSE )
		{	
		}
		free(data);*/
	} 
	/*else 
	{
	}*/
	return buffer;
}

void    ALSoundSystem::releaseSoundBuffer( SoundBufferInterface * _soundBuffer )
{
	if(!_soundBuffer) return;

	//_soundBuffer->unload();
	delete static_cast<ALSoundBuffer*>(_soundBuffer);
}

void    ALSoundSystem::releaseSoundNode( SoundSourceInterface * _sn )
{
	if(_sn)
	delete _sn;
}

void	ALSoundSystem::setSoundVelocity(float _velocity)
{
	mSoundVelocity = _velocity;
	alDopplerVelocity(mSoundVelocity);
}

void	ALSoundSystem::setDopplerFactor(float _factor)
{
	mDopplerFactor = _factor;
	alDopplerFactor(mDopplerFactor);
}

void	ALSoundSystem::setDistanceModel(DistanceModel _model)
{
	switch(_model) 
	{
	case(None):
	  alDistanceModel(AL_NONE);
	  break;
	case(InverseDistance):
	  alDistanceModel(AL_INVERSE_DISTANCE);
	  break;
	case(InverseDistanceClamped):
	  alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	  break;
	default:
		// TODO: Error handling
		return;
	}
	if(alGetError()!=AL_FALSE)
	{
	  // TODO: Error handling
		return;
	}

	mDistanceModel = _model;
}

UINT DecodeOggVorbis(OggVorbis_File* _oggVorbisFile, char* _decodeBuffer, UINT _bufferSize, UINT _channels)
{
	int currentSection;
	int decodeSize;
	UINT sampleNum;
	short* samples;

	UINT bytesDone = 0;
	while (decodeSize = ov_read(_oggVorbisFile, _decodeBuffer + bytesDone, _bufferSize - bytesDone, 0, 2, 1, &currentSection))
	{
		bytesDone += decodeSize;

		if (bytesDone >= _bufferSize)
			break;
	}
	
	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (_channels == 6)
	{		
		samples = (short*)_decodeBuffer;
		for (sampleNum = 0; sampleNum < (_bufferSize>>1); sampleNum += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			Swap(samples[sampleNum+1], samples[sampleNum+2]);
			Swap(samples[sampleNum+3], samples[sampleNum+5]);
			Swap(samples[sampleNum+4], samples[sampleNum+5]);
		}
	}

	return bytesDone;
}

void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

UINT GetSampleSize(ALenum format) 
{
  switch(format) 
  {
    case(AL_FORMAT_MONO8):
      return 1;
      break;
    case(AL_FORMAT_STEREO8):
    case(AL_FORMAT_MONO16):
      return 2;
      break;
    case(AL_FORMAT_STEREO16):
      return 4;
      break;
    default:
      return 0;
  }
}

ALuint ALSoundSystem::getFreeSource()
{
	for(int i = 0; i < m_sourceNum; i++)
	{
		ALint state;
		alGetSourcei(m_sources[i], AL_SOURCE_STATE, &state);
		if(state != AL_PLAYING)
			return m_sources[i];
	}
	return AL_INVALID;
}