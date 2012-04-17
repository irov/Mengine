#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "ResourceImage.h"

#	include "LogEngine.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
		: m_resourceAnimation(0)
		, m_frameTiming(0.f)
		, m_currentFrame(0)
		, m_onEndFrameTick(false)
		, m_onEndFrameEvent(false)
		, m_onEndAnimationEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Animation::~Animation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setAnimationResource( const ConstString& _resource )
	{
		if( m_resourceAnimationName == _resource ) 
		{
			return;
		}

		m_resourceAnimationName = _resource;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Animation::getAnimationResource() const
	{
		return m_resourceAnimationName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update( float _timing )
	{
		if( this->isPlay() == false )
		{
			return; 
		}

		size_t frameSize = m_resourceAnimation->getSequenceCount();

		float speedFactor = this->getSpeedFactor();
		m_frameTiming += _timing * speedFactor;

		float delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
		
//		float speedFactor = this->getSpeedFactor();
//		delay *= speedFactor;

		while( m_frameTiming >= delay )
		{
			m_frameTiming -= delay;

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
				if( this->getLoop() == true )
				{
					m_currentFrame = 0;
				}
				else
				{
					this->end();
					return;
				}
			}	

			delay = m_resourceAnimation->getSequenceDelay( m_currentFrame );

//			float speedFactor = this->getSpeedFactor();
//			delay *= speedFactor;
		}

		this->updateCurrentImageResource_();

		Sprite::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
		if( m_resourceAnimationName.empty() == true )
		{
			return false;
		}

		m_currentFrame = 0;

		if( m_resourceAnimation == NULL )
		{
			MENGE_LOG_ERROR( "Animation: '%s' Image resource not getting '%s'"
				, getName().c_str()
				, m_resourceAnimationName.c_str() 
				);

			return false;
		}

		if( this->updateCurrentImageResource_() == false )
		{
			return false;
		}
			
		if( Sprite::_activate() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( m_resourceAnimationName.empty() == true )
		{	
			MENGE_LOG_ERROR( "Animation: '%s' no set resource name"
				, m_name.c_str()
				);

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

		//m_currentFrame = 0;

		//this->setTiming( 0.f );
		//m_frameTiming = 0.0f;
		
		//if( this->updateCurrentImageResource_() == false )
		//{
		//	return false;
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		//set resource to NULL because sprite not create it
		//m_resource = NULL;
		
		Sprite::_release();

		if( m_resourceAnimation != 0 )
		{
			m_resourceAnimation->decrementReference();
			m_resourceAnimation = 0;
		}

		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_play()
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}
		
		this->setTiming( 0.f );
		//m_currentFrame = 0; 
		//m_timing = 0.f;
	
		//if( this->updateCurrentImageResource_() == false )
		//{
		//	return false;
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_restart( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}

		//this->setTiming( 0.f );
		//m_currentFrame = 0;
		//m_timing = 0.f;

		//if( this->updateCurrentImageResource_() == false )
		//{
		//	return;
		//}

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( size_t _enumerator )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation: '%s' end not activate"
				, getName().c_str()
				);

			return;
		}

		//this->setTiming( 0.f );
		//m_timing = 0.f;
		////m_currentFrame = m_resourceAnimation->getLastFrameIndex();
		//m_currentFrame = 0;

		//if( this->updateCurrentImageResource_() == false )
		//{
		//	return;
		//}

		if( m_onEndAnimationEvent == true )
		{
			this->callEvent( EVENT_ANIMATION_END, "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Animation::getFrame_( float _timing, float & _delthaTiming ) const
	{
		if( _timing <= 0.f )
		{
			_delthaTiming = _timing;

			return 0;
		}
				
		float duration = m_resourceAnimation->getSequenceDuration();

		if( _timing >= duration )
		{
			_timing -= floorf( _timing / duration ) * duration;

			if( _timing == 0.f )
			{
				_delthaTiming = 0.f;

				return 0;
			}
		}

		size_t count = m_resourceAnimation->getSequenceCount();
				 
		for( size_t frame = 0; frame != count; ++frame )
		{
			float delay = m_resourceAnimation->getSequenceDelay( frame );

			_timing -= delay;

			if( _timing <= 0.f )
			{
				_delthaTiming = _timing + delay;

				return frame;
			}
		}
		
		return count - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::updateCurrentImageResource_()
	{
		printf("Animation %s %d\n"
			, m_name.c_str()
			, m_currentFrame
			);

		if( m_resource != NULL )
		{
			m_resource->decrementReference();
		}

		m_resourceName =  m_resourceAnimation->getSequenceResourceName( m_currentFrame );
		m_resource =  m_resourceAnimation->getSequenceResource( m_currentFrame );
		m_resource->incrementReference();

		//MENGE_LOG_ERROR( "Animation: '%s'  name  resourceName %s ", m_name.c_str(), m_resourceName.c_str()	);
		
		if( Sprite::_compile() == false )
		{
			return false;
		}

		return true;
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
	size_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Animation::getFrameCount() const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameCount: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0;
		}

		return m_resourceAnimation->getSequenceCount();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( size_t  _frame ) const
	{
		if( this->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Animation.getFrameDelay: '%s' not compiled resource '%s'"
				, m_name.c_str()
				, m_resourceAnimationName.c_str()
				);

			return 0.f;
		}

		return m_resourceAnimation->getSequenceDelay( _frame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( size_t _frame )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation.setCurrentFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			MENGE_LOG_ERROR( "Animation.setCurrentFrame: '%s' _frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, _frame
				, sequenceCount
				);

			return;
		}

		m_currentFrame = _frame;

		this->updateCurrentImageResource_();
	}

	//////////////////////////////////////////////////////////////////////////
	void Animation::_setFirstFrame()
	{
		size_t firstFrame = 0;

		this->setCurrentFrame( firstFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setLastFrame()
	{
		size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		size_t lastFrame = sequenceCount - 1;

		this->setCurrentFrame( lastFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setTiming( float _timing )
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation._setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}
				
		m_currentFrame = this->getFrame_( _timing, m_frameTiming );

		this->updateCurrentImageResource_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::_getTiming() const
	{
		if( this->isActivate() == false )
		{
			MENGE_LOG_ERROR( "Animation._getTiming: '%s' not activate"
				, m_name.c_str()
				);

			return 0.f;
		}

		float timing = 0.f;

		for( size_t frame = 0; frame != m_currentFrame; ++frame )
		{
			float delay = m_resourceAnimation->getSequenceDelay( frame );

			timing += delay;
		}

		timing += m_frameTiming;

		return timing; 
	}
	//////////////////////////////////////////////////////////////////////////
}
