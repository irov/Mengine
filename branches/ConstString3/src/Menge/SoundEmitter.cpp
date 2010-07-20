#	include "SoundEmitter.h"

#	include "BinParser.h"

#	include "ResourceSound.h"

#	include "ResourceManager.h"

#	include "Logger/Logger.h"

#	include "SoundEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundEmitter::SoundEmitter()
		: m_resource(0)
		, m_sourceID( 0 )
		, m_isHeadMode(false)
		, m_looped( false )
		, m_playing( false )
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
			BIN_CASE_ATTRIBUTE( Protocol::Looping_Value, m_looped );
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
			->setLooped( m_sourceID, m_looped );

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
		m_playing = false;
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
		m_playing = false;
		callEvent( EVENT_SOUND_PAUSE, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::listenStopped()
	{
		m_playing = false;
		callEvent( EVENT_SOUND_STOP, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		m_playing = true;
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->play( m_sourceID );
		}

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::pause()
	{
		m_playing = false;
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->pause( m_sourceID );
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEmitter::stop()
	{
		m_playing = false;
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->stop( m_sourceID );
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isPlaying()
	{
		return m_playing;
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
	void SoundEmitter::setLooped( bool _loop )
	{
		m_looped = _loop;
		if( m_sourceID != 0 )
		{
			SoundEngine::get()
				->setLooped( m_sourceID, m_looped );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEmitter::isLooping()
	{
		return m_looped;
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
	void SoundEmitter::_setListener( PyObject * _listener )
	{
		Node::_setListener( _listener );

		Eventable::registerEvent( EVENT_SOUND_STOP, ("onStopped"), _listener );
		Eventable::registerEvent( EVENT_SOUND_PAUSE, ("onPaused"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
}
