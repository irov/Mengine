#	include "SoundSystem.h"

#	include "WavSoundBuffer.h"
#	include "OggSoundBuffer.h"

#	include "OpenAL/al.h"

bool initInterfaceSystem(SoundSystemInterface** _ptrSoundSystem)
{
	try
	{
		*_ptrSoundSystem = new OpenALSoundSystem();
	}

	catch (...)
	{
		return false;
	}
	
	return true;
}

void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<OpenALSoundSystem*>(_ptrSoundSystem);
}

OpenALSoundSystem::OpenALSoundSystem() 
	: SoundSystemInterface()
{
	m_pos[0] = 0.0f;
	m_pos[1] = 0.0f;
	m_pos[2] = 0.0f;

	m_updir[0] = 0.0f;
	m_updir[1] = 0.0f;
	m_updir[2] = 0.0f;
	m_updir[3] = 0.0f;
	m_updir[4] = 0.0f;
	m_updir[5] = 0.0f;

	m_deviceAL = alcOpenDevice(0);

	if (m_deviceAL)
	{
		m_contextAL = alcCreateContext(m_deviceAL, NULL);
		alcMakeContextCurrent(m_contextAL);
	}

	alcGetError(m_deviceAL);

	alListenerfv(AL_VELOCITY, m_pos);
	alListenerfv(AL_POSITION, m_pos);
	alListenerfv(AL_ORIENTATION, m_updir);
}

OpenALSoundSystem::~OpenALSoundSystem()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_contextAL);
	alcCloseDevice(m_deviceAL);
}

void	OpenALSoundSystem::setListenerOrient(const float* _position, const float* _updir)
{
	m_pos[0] = _position[0];
	m_pos[1] = _position[1];
	m_pos[2] = _position[2];

	m_updir[0] = _updir[0];
	m_updir[1] = _updir[1];
	m_updir[2] = _updir[2];
	m_updir[3] = _updir[0];
	m_updir[4] = _updir[1];
	m_updir[5] = _updir[2];

	alListenerfv( AL_POSITION, m_pos );
	alListenerfv( AL_ORIENTATION, m_updir );
}

SoundSourceInterface*	OpenALSoundSystem::loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener)
{
	SoundBufferInterface*	sb = NULL;

	if(_desc.type == WAV)
	{
		sb = new CALWavBuffer(_desc.buffer, _desc.size);
	}

	if(_desc.type == OGG)
	{
		sb = new OggSoundData(_desc.buffer, _desc.size);
	}

	return new OpenALSoundSource(_listener, sb, _desc.isStreamSound);
}

void	OpenALSoundSystem::releaseSoundNode(SoundSourceInterface*	_sn)
{
	delete ((OpenALSoundSource*)_sn);
}