#	include "Video.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceVideo.h"
#	include "ResourceSound.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"

namespace	Menge
{
	OBJECT_IMPLEMENT(Video)
		//////////////////////////////////////////////////////////////////////////
		Video::Video()
		: m_resourceVideo(0)
		, m_resourceSound( 0 )
		, m_soundSource( 0 )
		, m_playing(false)
		, m_autoStart(false)
		, m_looping(false)
		, m_needUpdate( false )
		, m_timing( 0.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	void Video::loader( XmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ResourceVideo"), MENGE_TEXT("Name"), m_resourceVideoName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ResourceSound"), MENGE_TEXT("Name"), m_resourceSoundName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Looping"), MENGE_TEXT("Value"), m_looping );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("AutoStart"), MENGE_TEXT("Value"), m_autoStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_setListener()
	{
		Sprite::_setListener();

		registerEvent( EVENT_VIDEO_END, ("onVideoEnd"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setAnimationResource( const String& _resource )
	{
		m_resourceVideoName = _resource;
		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::setLooped( bool _loop )
	{
		m_looping = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::getLooped() const
	{
		return m_looping;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_update( float _timing )
	{
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		m_timing += _timing;
		m_needUpdate = m_resourceVideo->sync( m_timing );

		if( m_resourceVideo->eof() == true )
		{
			stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		if( m_autoStart )
		{
			play_();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_deactivate()
	{
		Sprite::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Video::_compile()
	{
		/*if( Sprite::_compile() == false )
		{
			return false;
		}*/

		m_resourceVideo = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceVideo>( m_resourceVideoName );

		if( m_resourceVideo == 0 )
		{
			return false;
		}

		m_size = m_resourceVideo->getFrameSize();
		m_renderImage = Holder<RenderEngine>::hostage()
			->createImage( m_resourceVideoName, m_size.x, m_size.y );

		if( m_resourceSoundName.empty() == false )
		{
			m_resourceSound = Holder<ResourceManager>::hostage()
				->getResourceT<ResourceSound>( m_resourceSoundName );

			if( m_resourceSound )
			{
				SoundBufferInterface * soundBuffer = m_resourceSound->get();

				m_soundSource = Holder<SoundEngine>::hostage()
					->createSoundSource( true, soundBuffer, 0/*this*/ );
			}

			//Holder<SoundEngine>::hostage()->registerSoundEmitter( this );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_release()
	{
		Sprite::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceVideo );

		Holder<RenderEngine>::hostage()
			->releaseImage( m_renderImage );

		if( m_resourceSound )
		{

			Holder<SoundEngine>::hostage()
				->releaseSoundSource( m_soundSource );

			Holder<ResourceManager>::hostage()
				->releaseResource( m_resourceSound );

			m_soundSource = 0;
			m_resourceSound = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::stop()
	{
		if( m_playing == true )
		{
			m_playing = false;
			m_resourceVideo->seek( 0.0f );
			m_timing = 0.0f;
			if( m_soundSource )
			{
				m_soundSource->stop();
			}
			callEvent( EVENT_VIDEO_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::pause()
	{
		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::play_()
	{
		m_playing = true;
		if( m_soundSource )
		{
			m_soundSource->play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Video::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );
		if( m_needUpdate )
		{
			int pitch = 0;
			unsigned char* lockRect = m_renderImage->lock( &pitch, false );
			m_resourceVideo->getRGBData( lockRect, pitch );
			m_renderImage->unlock();
			m_needUpdate = false;
		}

		const mt::vec2f* vertices = getVertices();
		unsigned int color = m_color.getAsARGB();

		Holder<RenderEngine>::hostage()->renderImage(
			vertices,
			m_uv,
			color,
			m_renderImage,
			m_blendSrc,
			m_blendDest
			);
	}
	//////////////////////////////////////////////////////////////////////////
}