
#include "stdafx.h"

#include "AL/ALut.h"
#include "Vorbis/Vorbisfile.h"

#include "ALSoundBuffer.h"
#include "ALSoundBufferStream.h"
#include "ALSoundSource.h"
#include "ALSoundSystem.h"

//#include <stdlib.h>
#include <algorithm>

unsigned int DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned int ulBufferSize, unsigned int ulChannels);
void Swap(short &s1, short &s2);
unsigned int GetSampleSize(ALenum format);

ALSoundSystem::ALSoundSystem() :
m_soundVelocity(343.0f),
m_dopplerFactor(1.0f),
m_distanceModel(None),
m_sourceNamesNum(0)
{
	char *initString = 0L;//"DirectSound", "DirectSound3D", ;
	m_device = alcOpenDevice(NULL);

    if(!m_device)
	{
		// TODO: error handling
	}

    m_context = alcCreateContext(m_device, NULL);
    if(!m_context || alcGetError(m_device) != ALC_NO_ERROR) 
	{
      if(m_context)
	  {
	      alcDestroyContext(m_context);
	  }
      alcCloseDevice(m_device);

    } 
    alcMakeContextCurrent(m_context);
 
	for(;m_sourceNamesNum < MAX_SOURCENAMES_NUM; m_sourceNamesNum++)
	{
		ALuint sourceName;
		alGenSources(1, &sourceName);	
		if(alGetError())
			break;
		m_sourceNames[m_sourceNamesNum].busy = false;
		m_sourceNames[m_sourceNamesNum].name = sourceName;
	}
	m_sources.reserve(MAX_SOUND_SOURCES);
//	m_buffers.reserve(100);
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
	alSpeedOfSound(m_soundVelocity);
	//alDopplerVelocity(mSoundVelocity);
	alDopplerFactor(m_dopplerFactor);
	setDistanceModel(m_distanceModel);
}

ALSoundSystem::~ALSoundSystem()
{
	//alDeleteSources(m_sourceNum, m_sources);
	for(unsigned int i = 0; i < m_sources.size(); i++)
		delete m_sources[i];

	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}

void    ALSoundSystem::setListenerOrient( float * _position, float * _front, float * _top)
{
	memcpy( m_listenerPosition, _position, sizeof(float)*3 );
	memcpy( m_listenerOrientation, _front, sizeof(float)*3 );
	memcpy( m_listenerOrientation+3, _top, sizeof(float)*3 );

	alListenerfv(AL_POSITION, m_listenerPosition);
	alListenerfv(AL_ORIENTATION, m_listenerOrientation);
}

SoundSourceInterface*   ALSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface * _listener )
{
	//ALSoundSource* source = new ALSoundSource(this);
	ALSoundSource* source = NULL;
	for(unsigned int i = 0; i < m_sources.size(); i++)
		if(!m_sources[i]->isBusy())
		{
			source = m_sources[i];
			break;
		}

	if(!source)
	{
		source = new ALSoundSource(this);
		m_sources.push_back(source);
	}

	source->setSoundBuffer( static_cast<ALSoundBuffer*>(_sample) );
	source->setAmbient(_isHeadMode);
	source->setSoundNodeListener(_listener);
	source->setUsed(true);

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
			FILE* filehandle = NULL;
			fopen_s(&filehandle, _filename, "rb");

			if( ov_open( filehandle, &oggfile, NULL, 0 ) >= 0 ) 
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

				buffer->setLenghtMs(static_cast<unsigned int>( ov_time_total(&oggfile, -1) * 1000 ));

				size = ov_pcm_total(&oggfile, -1) * GetSampleSize(format);

				data = malloc(size);
				DecodeOggVorbis(&oggfile, (char*)data, size, ogginfo->channels);

			} 
			else
			{
				delete buffer;
				buffer = NULL;
			}
			ov_clear(&oggfile);
			fclose(filehandle);
		}

		if(data) 
		{
			alBufferData( buffer->getBufferName(), format, data, size, freq );
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
	} 
	return buffer;
}

void    ALSoundSystem::releaseSoundBuffer( SoundBufferInterface * _soundBuffer )
{
	//_soundBuffer->unload();
	delete static_cast<ALSoundBuffer*>(_soundBuffer);
}

void    ALSoundSystem::releaseSoundNode( SoundSourceInterface * _sn )
{
//	delete _sn;
	if(_sn)
	{
		static_cast<ALSoundSource*>(_sn)->setUsed(false);
		static_cast<ALSoundSource*>(_sn)->stop();
	}
}

void ALSoundSystem::update()
{
	for(unsigned int i = 0; i < m_streams.size(); i++)
	{
		m_streams[i]->getUpdater()->update();
	}
}

void	ALSoundSystem::setSoundVelocity(float _velocity)
{
	m_soundVelocity = _velocity;
	alDopplerVelocity(m_soundVelocity);
}

void	ALSoundSystem::setDopplerFactor(float _factor)
{
	m_dopplerFactor = _factor;
	alDopplerFactor(m_dopplerFactor);
}

void	ALSoundSystem::setDistanceModel(EDistanceModel _model)
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
	m_distanceModel = _model;
}

void ALSoundSystem::addStream( ALSoundBufferStream *_stream )
{
	m_streams.push_back( _stream );
}

void ALSoundSystem::removeStream( ALSoundBufferStream* _stream )
{
	TVectorALSoundBufferStream::iterator it = std::find( m_streams.begin(), m_streams.end(), _stream );
	if(it != m_streams.end())
		m_streams.erase(it);
}

unsigned int DecodeOggVorbis(OggVorbis_File* _oggVorbisFile, char* _decodeBuffer, unsigned int _bufferSize, unsigned int _channels)
{
	int currentSection;
	int decodeSize;
	unsigned int sampleNum;
	short* samples;

	unsigned int bytesDone = 0;
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

unsigned int GetSampleSize(ALenum format) 
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

TALSourceName* ALSoundSystem::getFreeSourceName()
{
	for(int i = 0; i < m_sourceNamesNum; i++)
	{
		if(!m_sourceNames[i].busy)
		{
			return &m_sourceNames[i];
		}
	}

	for(int i = 0; i < m_sourceNamesNum; i++)
	{
		int state;
		alGetSourcei(m_sourceNames[i].name, AL_SOURCE_STATE, &state);
		if(state != AL_PLAYING)
		{
			return &m_sourceNames[i];
		}
	}
	
	return 0;
}