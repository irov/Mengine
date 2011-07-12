#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "BinParser.h"

#	include "LogEngine.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
	: m_resourceAnimation(0)
	, m_playing(false)
	, m_autoStart(false)
	, m_randomStart(false)
	, m_looping(false)
	, m_delay(0)
	, m_currentFrame(0)
	, m_onEndFrameTick(false)
	, m_onEndFrameEvent(false)
	, m_onEndAnimationEvent(false)
	, m_animationFactor(1.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Animation::loader( BinParser * _parser )
	{
		Sprite::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Animation_Name, m_resourceAnimationName );
			BIN_CASE_ATTRIBUTE( Protocol::Looping_Value, m_looping );
			BIN_CASE_ATTRIBUTE( Protocol::AutoStart_Value, m_autoStart );			
			BIN_CASE_ATTRIBUTE( Protocol::RandomStart_Value, m_randomStart );			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const ConstString& _resource )
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
	void Animation::setAutoPlay( bool _value )
	{
		m_autoStart = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::getAutoPlay() const
	{
		return m_autoStart;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update( float _timing )
	{
		Sprite::_update( _timing );

		if( m_playing == false )
		{
			return; 
		}

		std::size_t frameSize = m_resourceAnimation->getSequenceCount();

		m_delay += _timing;

		float delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
		delay *= m_animationFactor;

		while( m_delay >= delay )
		{
			m_delay -= delay;
			
			if( m_onEndFrameEvent == true )
			{
				this->callEvent( EVENT_FRAME_END, "(OI)", this->getEmbed(), m_currentFrame );
			}

			if( m_onEndFrameTick == true )
			{
				this->callEvent( EVENT_FRAME_TICK, "(OII)", this->getEmbed(), m_currentFrame, frameSize );
			}
			else
			{
				++m_currentFrame;
			}

			if( m_currentFrame == frameSize )
			{
				if( m_looping == false )
				{
					m_playing = false;
					--m_currentFrame;

					if( m_onEndAnimationEvent == true )
					{
						this->callEvent( EVENT_ANIMATION_END, "(OO)", this->getEmbed(), pybind::ret_bool(true) );
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
				MENGE_LOG_ERROR( "Animation: '%s' Image resource not getting '%s'"
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
	bool Animation::_compile()
	{
		if( Sprite::_compile() == false )
		{
			return false;
		}

		m_resourceAnimation = ResourceManager::get()
			->getResourceT<ResourceAnimation>( m_resourceAnimationName );

		if( m_resourceAnimation == 0 )
		{
			MENGE_LOG_ERROR( "Animation: '%s' no found resource with name '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str() 
				);

			return false;
		}

		if( m_randomStart )
		{			
			std::size_t sequenceCount = m_resourceAnimation->getSequenceCount();

			m_currentFrame = mt::rand( sequenceCount-1 );

			float sequenceDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
			m_delay = mt::range_randf( 0.0f, sequenceDelay );
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

		ResourceManager::get()
			->releaseResource( m_resourceAnimation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::stop()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}

		this->stop_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::pause()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' pause not activate"
				, getName().c_str()
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
			MENGE_LOG_ERROR( "Animation: '%s' resume not activate"
				, getName().c_str()
				);

			return;
		}	

		this->resume_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::play()
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' play not activate"
				, getName().c_str()
				);

			return;
		}

		this->stop_();
		this->resume_();
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
				this->callEvent( EVENT_ANIMATION_END, "(OO)", this->getEmbed(), pybind::ret_bool(false) );
			}
		}

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex(m_currentFrame);
		this->setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::resume_()
	{
		m_playing = true;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		setImageIndex( currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setEventListener( PyObject * _listener )
	{
		Sprite::_setEventListener( _listener );

		Eventable::registerEvent( EVENT_FRAME_END, ("onFrameEnd"), _listener, &m_onEndFrameEvent );
		Eventable::registerEvent( EVENT_FRAME_TICK, ("onFrameTick"), _listener, &m_onEndFrameTick );

		Eventable::registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), _listener, &m_onEndAnimationEvent );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Animation::getFrameCount() const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' getFrameCount not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( std::size_t  _frame ) const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' getFrameDelay not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceDelay( _frame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( std::size_t _frame )
	{
		if( isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' setCurrentFrame not activate"
				, m_name.c_str()
				);

			return;
		}

		std::size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			MENGE_LOG_ERROR( "Animation: '%s' setCurrentFrame _frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, _frame
				, sequenceCount
				);

			return;
		}

		m_currentFrame = _frame;

		std::size_t currentImageIndex = m_resourceAnimation->getSequenceIndex( m_currentFrame );
		this->setImageIndex( currentImageIndex );
	}
}
