#	include "ALSoundSystem.h"

#	include "ALWavSound.h"
#	include "ALOggStreamSound.h"

#	include	"assert.h"

bool initInterfaceSystem(SoundSystemInterface** _ptrSoundSystem)
{
	try
	{
		*_ptrSoundSystem = new ALSoundSystem();
	}

	catch (...)
	{
		return false;
	}

	return true;
}

void releaseInterfaceSystem(SoundSystemInterface* _ptrSoundSystem)
{
	delete static_cast<ALSoundSystem*>(_ptrSoundSystem);
}

ALSoundSystem::ALSoundSystem() 
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

	m_deviceAL = alcOpenDevice(NULL);

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

ALSoundSystem::~ALSoundSystem()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_contextAL);
	alcCloseDevice(m_deviceAL);
}

void	ALSoundSystem::checkError()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		int p =0;
		assert(!"error");
	}
}

void	ALSoundSystem::setListenerOrient(const float* _position, const float* _updir)
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

SoundSourceInterface*	ALSoundSystem::loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener)
{
	if(_desc.type == SoundDataDesc::WAV)
	{
		return new WavSound(_desc.buffer,_listener);
	}

	if(_desc.type == SoundDataDesc::OGG)
	{
		return new OggStreamSound(_desc.buffer, _desc.size,_listener);
	}
}

void	ALSoundSystem::releaseSoundNode(SoundSourceInterface*	_sn)
{
	delete _sn;
}
