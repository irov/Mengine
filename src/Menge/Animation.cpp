#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "ResourceImage.h"

#	include "Logger/Logger.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
		: m_frameTiming(0.f)
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
	void Animation::setResourceAnimation( ResourceAnimation * _resourceAnimation )
	{
		if( m_resourceAnimation == _resourceAnimation ) 
		{
			return;
		}

		m_resourceAnimation = _resourceAnimation;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation * Animation::getResourceAnimation() const
	{
		return m_resourceAnimation;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return; 
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

		size_t frameCount = m_resourceAnimation->getSequenceCount();

		float speedFactor = this->getSpeedFactor();
		float scretch = this->getScretch();
		m_frameTiming += _timing * speedFactor / scretch;

		float frameDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
        
		size_t lastFrame = m_currentFrame;

        if( m_currentFrame != frameCount )
        {
            while( m_frameTiming >= frameDelay )
            {
                m_frameTiming -= frameDelay;

                if( m_onEndFrameEvent == true )
                {
                    EVENTABLE_CALL(m_serviceProvider, this, EVENT_FRAME_END)( "(OI)", this->getEmbed(), m_currentFrame );
                }

                if( m_onEndFrameTick == true )
                {
                    EVENTABLE_CALL(m_serviceProvider, this, EVENT_FRAME_TICK)( "(OII)", this->getEmbed(), m_currentFrame, frameCount );
                }
                else
                {
                    ++m_currentFrame;
                }

                if( m_currentFrame == frameCount )
                {
                    if( this->getLoop() == true )
                    {
                        this->setTiming( m_frameTiming );                                   
                    }
                    else
                    {
                        if( --m_playIterator == 0 )
                        {
                            m_currentFrame = frameCount - 1;
                            this->updateCurrentFrame_();

                            m_frameTiming = 0.f;

                            lastFrame = m_currentFrame;

                            this->end();

                            break;
                        }
                        else
                        {
                            this->setTiming( m_frameTiming );
                        }					
                    }

                    lastFrame = m_currentFrame;
                }

                frameDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );                
            }
        }

		if( lastFrame != m_currentFrame )
		{
			this->updateCurrentFrame_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_render( RenderCameraInterface * _camera )
	{
		//printf("Animation::_render %s %d %f\n"
		//	, m_name.c_str()
		//	, m_currentFrame
		//	, this->getTiming()
		//	);

		Sprite::_render( _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_activate()
	{
        this->setTiming( 0.f );
			
		if( Sprite::_activate() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_deactivate()
	{
		this->stop();

		Sprite::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_compile()
	{
		if( m_resourceAnimation == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceAnimation.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Animation::_compile: '%s' resource '%s' is not compile"
                , m_name.c_str()
                , m_resourceAnimation->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_release()
	{
		Sprite::_release();

        m_resourceAnimation.release();

		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_play( float _time )
	{
        (void)_time;

		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}
		        
        m_playIterator = this->getPlayCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_restart( float _time, size_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

        m_playIterator = this->getPlayCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( size_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}
        
		if( m_onEndAnimationEvent == true )
		{
			EVENTABLE_CALL(m_serviceProvider, this, EVENT_ANIMATION_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(false) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( size_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation: '%s' end not activate"
				, getName().c_str()
				);

			return;
		}
        
		if( m_onEndAnimationEvent == true )
		{
			EVENTABLE_CALL(m_serviceProvider, this, EVENT_ANIMATION_END)( "(OiO)", this->getEmbed(), _enumerator, pybind::get_bool(true) );
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

            if( fabsf(_timing) < 0.0001f )
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

			if( _timing < 0.f )
			{
				_delthaTiming = _timing + delay;

				return frame;
			}
		}
		
		return count - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::updateCurrentFrame_()
	{
        Sprite::_release();
        
		m_resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );
        	
		if( Sprite::_compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation::updateCurrentFrame_ '%s' invalid compile %d frame"
                , this->getName().c_str()
                , m_currentFrame
                );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setEventListener( PyObject * _listener )
	{
		Sprite::_setEventListener( _listener );

		this->registerEvent( EVENT_FRAME_END, ("onFrameEnd"), _listener, &m_onEndFrameEvent );
		this->registerEvent( EVENT_FRAME_TICK, ("onFrameTick"), _listener, &m_onEndFrameTick );

		this->registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), _listener, &m_onEndAnimationEvent );
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
			LOGGER_ERROR(m_serviceProvider)( "Animation.getFrameCount: '%s' not compiled resource"
				, m_name.c_str()
				);

			return 0;
		}

        size_t count = m_resourceAnimation->getSequenceCount();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( size_t  _frame ) const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation.getFrameDelay: '%s' not compiled resource"
				, m_name.c_str()
				);

			return 0.f;
		}

		float delay = m_resourceAnimation->getSequenceDelay( _frame );

        return delay;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( size_t _frame )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation.setCurrentFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		size_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation.setCurrentFrame: '%s' _frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, _frame
				, sequenceCount
				);

			return;
		}

		m_currentFrame = _frame;
		m_frameTiming = 0.f;

		this->updateCurrentFrame_();
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
	void Animation::_setReverse( bool _value )
	{
        (void)_value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setTiming( float _timing )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation._setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}
				
		m_currentFrame = this->getFrame_( _timing, m_frameTiming );

		this->updateCurrentFrame_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::_getTiming() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Animation._getTiming: '%s' not activate"
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
	bool Animation::_interrupt( size_t _enumerator )
	{
        (void)_enumerator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}
