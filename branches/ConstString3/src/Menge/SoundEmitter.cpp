#	include "SoundEmitter.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "SoundEngine.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_resource(0)
		, m_sourceID(0)
		, m_isHeadMode(false)
		, m_onSoundPauseEvent(false)
		, m_onSoundStopEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::~SoundEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = 
			ResourceManager::get()
			->getResourceT<ResourceSound>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Error: sound emitter '%s' can't get resource '%s'"
				, getName().c_str()
				, m_resourcename.c_str()
				);

			return false;
		}

		SoundBufferInterface * soundBuffer = m_resource->getSoundBuffer();

		m_sourceID = SoundEngine::get()
			->createSoundSource( 
			m_isHeadMode
			, soundBuffer
			, m_resource->isStreamable()
			);

		if( m_sourceID == 0 )
		{
			MENGE_LOG_ERROR( "Warning: sound emitter '%s' not compiled"
				, getName().c_str() 
				);

			return false;
		}

		SoundEngine::get()
			->setSourceListener( m_sourceID, this );

		SoundEngine::get()
			->setLoop( m_sourceID, m_loop );

		float volume = m_resource->getDefaultVolume();
		this->setVolume( volume );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_release()
	{ 
		SoundEngine::get()
			->releaseSoundSource( m_sourceID );

		m_sourceID = 0;

		if( m_resource != 0 )
		{
			m_resource->decrementReference();
			m_resource = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setSoundResource( const ConstString& _name )
	{
		if( m_resourcename == _name )
		{
			return;
		}

		m_resourcename = _name;

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenSoundNodePaused()
	{
		if( m_onSoundPauseEvent == true )
		{
			this->callEvent( EVENT_SOUND_PAUSE, "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenSoundNodeStopped()
	{
		if( m_onSoundStopEvent )
		{
			this->end();
			//this->callEvent( EVENT_SOUND_STOP, "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_play()
	{
		if( isActivate() == false )
		{
			return false;
		}

		if( m_sourceID == 0 )
		{
			return false;
		}

		SoundEngine::get()
			->play( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::_restart( size_t _enumerator )
	{
		//ToDo

		return false;
	}
	////////////////////////////////////////////////////////////////////////////
	//void SoundEmitter::pause()
	//{
	//	m_playing = false;
	//	if( m_sourceID != 0 )
	//	{
	//		SoundEngine::get()
	//			->pause( m_sourceID );
	//	}
	//	return;
	//}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_stop( size_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->stop( m_sourceID );
		}

		this->callEventDeferred( EVENT_SOUND_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_end( size_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->stop( m_sourceID );
		}

		this->callEventDeferred( EVENT_SOUND_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setVolume( float _volume )
	{
		if( m_sourceID == 0 )
		{
			return;
		}
		
		if( SoundEngine::get()
			->setSourceVolume( m_sourceID, _volume ) == false )
		{
			MENGE_LOG_ERROR("SoundEmitter::setVolume invalid sourceID %s"
				, m_resourcename.c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getVolume()
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float volume = SoundEngine::get()
			->getSourceVolume( m_sourceID );		

		return volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setLoop( bool _value )
	{
		if( m_sourceID == 0 )
		{
			return;
		}
		
		SoundEngine::get()
			->setLoop( m_sourceID, m_loop );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getLengthMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float lengthMs = SoundEngine::get()
			->getLengthMs( m_sourceID );
	
		return lengthMs;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		Eventable::registerEvent( EVENT_SOUND_PAUSE, ("onSoundPause"), _listener, &m_onSoundPauseEvent );
		Eventable::registerEvent( EVENT_SOUND_END, ("onSoundEnd"), _listener, &m_onSoundStopEvent );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_interrupt( size_t _enumerator )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}
