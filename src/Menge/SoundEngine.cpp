#	include "SoundEngine.h"

#	include "SoundEmitter.h"

namespace Menge
{
	SoundEngine::SoundEngine( SoundSystemInterface * _interface )
	: m_interface( _interface )
	, m_soundVolume( 1.0f )
	, m_commonVolume( 1.0f )
	{
		Holder<SoundEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundEngine::setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top )
	{
		m_interface->setListenerOrient((float*)_position.m,(float*)_front.m,(float*)top.m);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface *	SoundEngine::createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			SoundNodeListenerInterface *	_listener = 0)
	{

		SoundSourceInterface* sound = m_interface->createSoundSource(_isHeadMode,_sample,_listener);
		return	sound;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface *	SoundEngine::createSoundBufferFromFile( const char* _filename, bool _isStream )
	{
		SoundBufferInterface * sample =  m_interface->createSoundBufferFromFile(_filename, _isStream);
		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface *	SoundEngine::createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem )
	{
		SoundBufferInterface * sample =  m_interface->createSoundBufferFromMemory(_buffer, _size, _newmem);
		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		m_interface->releaseSoundBuffer(_soundBuffer);
		_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource( SoundSourceInterface* _soundSource )
	{
		m_interface->releaseSoundNode(_soundSource);
		_soundSource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundSourceVolume( float _volume )
	{
		m_soundVolume = _volume;
		for( TSetSoundEmitters::iterator it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it )
		{
			float vol = (*it)->getVolume();
			(*it)->setVolume( vol );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::registerSoundEmitter( SoundEmitter * _emitter )
	{
		printf("Sound Registered! \n");
		m_soundEmitters.insert( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::unregisterSoundEmitter( SoundEmitter * _emitter )
	{
		printf("Sound UnRegistered! \n");
		m_soundEmitters.erase( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundSourceVolume() const
	{
		return m_soundVolume;
	}
	//////////////////////////////////////////////////////////////////////////	
	void SoundEngine::setCommonVolume( float _volume )
	{
		m_commonVolume = _volume;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume() const
	{
		return m_commonVolume;
	}
	//////////////////////////////////////////////////////////////////////////
}