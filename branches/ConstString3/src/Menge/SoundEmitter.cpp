#	include "SoundEmitter.h"

#	include "BinParser.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "Logger/Logger.h"

#	include "SoundEngine.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_resource(0)
		, m_sourceID(0)
		, m_isHeadMode(false)
		, m_loop(false)
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
	void SoundEmitter::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Resource_Name, m_resourcename );
			BIN_CASE_ATTRIBUTE( Protocol::HeadMode_Value, m_isHeadMode );
			BIN_CASE_ATTRIBUTE( Protocol::Loop_Value, m_loop );
		}
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

		SoundBufferInterface * soundBuffer = m_resource->get();

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

		ResourceManager::get()
			->releaseResource( m_resource );

		m_sourceID = 0;
		m_resource = NULL;
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
	void SoundEmitter::listenPaused()
	{
		if( m_onSoundPauseEvent == true )
		{
			this->callEvent( EVENT_SOUND_PAUSE, "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenStopped()
	{
		if( m_onSoundStopEvent )
		{
			this->stop();
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

		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->play( m_sourceID );
		}

		return true;
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
	void SoundEmitter::_stop( std::size_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->stop( m_sourceID );
		}

		this->callEventDeferred( EVENT_SOUND_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(false) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_end( std::size_t _enumerator )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->stop( m_sourceID );
		}

		this->callEventDeferred( EVENT_SOUND_END, "(OiO)", this->getEmbed(), _enumerator, pybind::ret_bool(true) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setVolume( float _volume )
	{
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->setVolume( m_sourceID, _volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getVolume()
	{
		if( m_sourceID != 0 )
		{
			return SoundEngine::get()
				->getVolume( m_sourceID );
		}
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::setLoop( bool _loop )
	{
		m_loop = _loop;
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->setLoop( m_sourceID, m_loop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::getLoop()
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEmitter::getLengthMs()
	{
		if( m_sourceID != 0 )
		{
			return SoundEngine::get()
				->getLengthMs( m_sourceID );
		}
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		m_onSoundPauseEvent = Eventable::registerEvent( EVENT_SOUND_PAUSE, ("onSoundPause"), _listener );
		m_onSoundStopEvent = Eventable::registerEvent( EVENT_SOUND_END, ("onSoundEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
}
