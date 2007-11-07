#	include "SoundEngine.h"

#	include "FileEngine.h"

#	include "SoundEmitter.h"

#	include "Interface/SoundSystemInterface.h"

#	include	<assert.h>

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
	void	SoundEngine::setListenerOrient(const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top)
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
	SoundBufferInterface * SoundEngine::createSoundBuffer()
	{
		SoundBufferInterface * sample = m_interface->createSoundBuffer();
		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer(SoundBufferInterface* _soundBuffer)
	{
		m_interface->releaseSoundBuffer(_soundBuffer);
		_soundBuffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource(SoundSourceInterface* _soundSource)
	{
		m_interface->releaseSoundNode(_soundSource);
		_soundSource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundSourceVolume( float _vol )
	{
		m_soundVolume = _vol;
		for( std::set<SoundEmitter*>::iterator it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it ) 		
		{
			float vol = (*it)->getVolume();
			(*it)->setVolume( vol );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::registerSoundEmitter( SoundEmitter * _emitter )
	{
		m_soundEmitters.insert( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::unregisterSoundEmitter( SoundEmitter * _emitter )
	{
		std::set<SoundEmitter*>::iterator it = m_soundEmitters.find( _emitter );

		if( it != m_soundEmitters.end() )
		{
			m_soundEmitters.erase( it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundSourceVolume() const
	{
		return m_soundVolume;
	}
	//////////////////////////////////////////////////////////////////////////	
	void SoundEngine::setCommonVolume( float _vol )
	{
		m_commonVolume = _vol;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume() const
	{
		return m_commonVolume;
	}
	//////////////////////////////////////////////////////////////////////////
}