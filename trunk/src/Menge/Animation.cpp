#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "XmlEngine.h"
#	include "LogEngine.h"

#	include "Math/rand.h"
namespace	Menge
{
	FACTORABLE_IMPLEMENT(Animation)
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
	void Animation::setAnimationResource( const String& _resource )
	{
		if( m_resourceAnimationName == _resource ) 
		{
			return;
		}
		
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
			MENGE_LOG_ERROR( "Sprite '%s': Image resource not getting "
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
			
			std::size_t frameSize = m_resourceAnimation->getSequenceCount();

			if( m_onEndFrameEvent == true )
			{
				callEvent( EVENT_FRAME_END, "(OI)", this->getEmbedding(), m_currentFrame );
			}

			if( ++m_currentFrame == frameSize )
			{
				if( m_looping == false )
				{
					m_playing = false;
					--m_currentFrame;

					if( m_onEndAnimationEvent == true )
					{
						callEvent( EVENT_ANIMATION_END, "(O)", this->getEmbedding() );
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

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
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
			resume_();
		}
		else
		{
			m_currentFrame = 0;

			if( m_resourceAnimation == NULL )
			{
				MENGE_LOG_ERROR( "Sprite '%s': Image resource not getting "
					, getName().c_str()
					, m_resourceAnimationName.c_str() 
					);
			}

			std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
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
			MENGE_LOG_ERROR( "Animation: no found resource with name '%s'"
				, m_resourceAnimationName.c_str() 
				);

			return false;
		}

		if( m_randomStart )
		{
			std::size_t sequenceCount = m_resourceAnimation->getSequenceCount();
			m_currentFrame = mt::rand( sequenceCount-1 );

			float sequenceDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
			m_delay = mt::even_rand( 0.0f, sequenceDelay );
		}
		else
		{
			m_currentFrame = 0;
			m_delay = 0.0f;
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
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.stop: not activate '%s'"
				, m_name.c_str()
				);

			return;
		}

		stop_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::pause()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.pause: not activate '%s'"
				, m_name.c_str()
				);

			return;
		}

		m_playing = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::resume()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.play: not activate '%s'"
				, m_name.c_str()
				);

			return;
		}	

		resume_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.play: not activate '%s'"
				, m_name.c_str()
				);

			return;
		}

		stop_();
		resume_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop_()
	{
		m_currentFrame = 0;
		m_delay = 0;

		if( m_playing == true )
		{
			m_playing = false;

			if( m_onEndAnimationEvent == true )
			{
				callEvent( EVENT_ANIMATION_END, "(O)", this->getEmbedding() );
			}
		}

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex(m_currentFrame);
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::resume_()
	{
		m_playing = true;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setListener()
	{
		Sprite::_setListener();

		m_onEndAnimationEvent = Eventable::registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), m_listener );
		m_onEndFrameEvent = Eventable::registerEvent( EVENT_FRAME_END, ("onFrameEnd"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getFrameCount() const
	{
		if( m_resourceAnimation )
		{
			MENGE_LOG_ERROR( "Animation.getFrameCount: not compiled resource '%s'"
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( std::size_t _frame )
	{
		std::size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame < sequenceCount )
		{
			m_currentFrame = _frame;
			std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
			setImageIndex( currentImageIndex );
		}
		else
		{
			MENGE_LOG_ERROR( "Animation::setCurrentFrame _frame(%d) >= sequenceCount(%d)"
				, _frame
				, sequenceCount
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
