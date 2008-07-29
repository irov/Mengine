#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"

#	include "Math/rand.h"
namespace	Menge
{
	OBJECT_IMPLEMENT(Animation)
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_resourceAnimation(0)
	, m_playing(false)
	, m_autoStart(false)
	, m_randomStart( false )
	, m_looping(false)
	, m_delay(0)
	, m_currentFrame(0)
	, m_onEndFrameEvent(false)
	, m_onEndAnimationEvent(false)
	, m_animationFactor(1.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( XmlElement * _xml )
	{
		Sprite::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Animation", "Name", m_resourceAnimationName );
			XML_CASE_ATTRIBUTE_NODE( "Looping", "Value", m_looping );
			XML_CASE_ATTRIBUTE_NODE( "AutoStart", "Value", m_autoStart );			
			XML_CASE_ATTRIBUTE_NODE( "RandomStart", "Value", m_randomStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const std::string & _resource )
	{
		if( m_resourceAnimationName == _resource ) return;
		m_resourceAnimationName = _resource;
		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationFactor( float _factor )
	{
		m_animationFactor = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getAnimationFactor() const
	{
		return m_animationFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setLooped( bool _loop )
	{
		m_looping = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::getLooped() const
	{
		return m_looping;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update( float _timing )
	{
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		if( m_resourceAnimation == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourceAnimationName.c_str() 
				);
		}

		m_delay += _timing;

		float delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
		delay *= m_animationFactor;

		while( m_delay >= delay )
		{
			m_delay -= delay;
			
			unsigned int frameSize = m_resourceAnimation->getSequenceCount();

			if( m_onEndFrameEvent == true )
			{
				callEvent( "END_FRAME", "(OI)", this->getEmbedding(), m_currentFrame );
			}

			if( ++m_currentFrame == frameSize )
			{
				if( m_looping == false )
				{
					m_playing = false;
					--m_currentFrame;

					if( m_onEndAnimationEvent == true )
					{
						callEvent( "END_ANIMATION", "(O)", this->getEmbedding() );
					}

					break;
				}
				else
				{
					m_currentFrame = 0;
				}
			}	
			delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
			delay *= m_animationFactor;
		}

		unsigned int currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );

	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( Sprite::_activate() == false )
		{
			return false;
		}

		if( m_autoStart )
		{
			play_();
		}
		else
		{
			m_currentFrame = 0;

			if( m_resourceAnimation == NULL )
			{
				MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
					, getName().c_str()
					, m_resourceAnimationName.c_str() 
					);
			}

			unsigned int currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
			setImageIndex( currentImageIndex );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_deactivate()
	{
		Sprite::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( Sprite::_compile() == false )
		{
			return false;
		}

		m_resourceAnimation = Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimation>( m_resourceAnimationName );

		if( m_resourceAnimation == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		Sprite::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceAnimation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop()
	{
		m_currentFrame = 0;
		m_delay = 0;

		if( m_playing == true )
		{
			m_playing = false;
			
			if( m_onEndAnimationEvent == true )
			{
				callEvent( "END_ANIMATION", "(O)", this->getEmbedding() );
			}
		}

		unsigned int currentImageIndex = m_resourceAnimation->getSequenceIndex(m_currentFrame);
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::pause()
	{
		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play()
	{
		if( isActivate() == false )
		{
			return;
		}

		play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play_()
	{
		m_playing = true;
		
		if( m_randomStart )
		{
			m_currentFrame = mt::even_rand( 0.0f, m_resourceAnimation->getSequenceCount() );
			m_delay = mt::even_rand( 0.0f, m_resourceAnimation->getSequenceDelay( m_currentFrame ) ) ;
		}
		else
		{
			m_currentFrame = 0;
			m_delay = 0.0f;
		}

		unsigned int currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setListener()
	{
		Sprite::_setListener();

		m_onEndAnimationEvent = registerEvent("END_ANIMATION", "onAnimationEnd", m_listener );
		m_onEndFrameEvent = registerEvent("END_FRAME", "onFrameEnd", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
}