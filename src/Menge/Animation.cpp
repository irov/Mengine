#	include "Animation.h"

#	include "ResourceManager.h"

#	include "ResourceAnimation.h"

#	include "Kernel/ResourceImage.h"

#	include "Logger/Logger.h"

#	include "Math/rand.h"

#	include "pybind/system.hpp"

#	include <math.h>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Animation::Animation()
		: m_frameTiming(0.f)
		, m_currentFrame(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Animation::~Animation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setResourceAnimation( const ResourceAnimationPtr & _resourceAnimation )
	{
		if( m_resourceAnimation == _resourceAnimation ) 
		{
			return;
		}

		m_resourceAnimation = _resourceAnimation;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceAnimationPtr & Animation::getResourceAnimation() const
	{
		return m_resourceAnimation;
	}
	//////////////////////////////////////////////////////////////////////////
	enum AnimationEventFlag
	{
		EVENT_FRAME_END = 0,
		EVENT_FRAME_TICK,
		EVENT_ANIMATION_END,
		EVENT_ANIMATABLE_END
	};
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setEventListener( const pybind::dict & _listener )
	{
		Sprite::_setEventListener( _listener );

		this->registerEvent( EVENT_FRAME_END, ("onFrameEnd"), _listener );
		this->registerEvent( EVENT_FRAME_TICK, ("onFrameTick"), _listener );
		this->registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), _listener );

		this->registerEvent( EVENT_ANIMATABLE_END, ("onAnimatableEnd"), _listener );
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

		uint32_t frameCount = m_resourceAnimation->getSequenceCount();

		float speedFactor = this->getAnimationSpeedFactor();
		float scretch = this->getStretch();
		m_frameTiming += _timing * speedFactor / scretch;

		float frameDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );
        
		uint32_t lastFrame = m_currentFrame;

        if( m_currentFrame != frameCount )
        {
            while( m_frameTiming >= frameDelay )
            {
                m_frameTiming -= frameDelay;

				EVENTABLE_CALL( m_serviceProvider, this, EVENT_FRAME_END )(this, m_currentFrame);
				
				++m_currentFrame;

				EVENTABLE_CALL( m_serviceProvider, this, EVENT_FRAME_TICK )(this, m_currentFrame, frameCount);

                if( m_currentFrame == frameCount )
                {
					bool loop = this->getLoop();
					bool interrupt = this->isInterrupt();

					if( (loop == false && --m_playIterator == 0) || interrupt == true )
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
	void Animation::_render( const RenderObjectState * _state )
	{
		Sprite::_render( _state );
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
			LOGGER_ERROR(m_serviceProvider)("Animation::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceAnimation.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Animation::_compile: '%s' resource '%s' is not compile"
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
			LOGGER_ERROR(m_serviceProvider)("Animation: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_restart( float _time, uint32_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_stop( uint32_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}
        
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATION_END )(this, _enumerator, false);
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, false);
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_end( uint32_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation: '%s' end not activate"
				, getName().c_str()
				);

			return;
		}
        
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATION_END )(this, _enumerator, true);
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ANIMATABLE_END )(this, _enumerator, true);
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Animation::getFrame_( float _timing, float & _delthaTiming ) const
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

		uint32_t count = m_resourceAnimation->getSequenceCount();

		for( uint32_t frame = 0; frame != count; ++frame )
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
		if( m_resourceImage != nullptr )
		{
			Sprite::_release();
		}
        
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
	uint32_t Animation::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Animation::getFrameCount() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation.getFrameCount: '%s' not compiled resource"
				, m_name.c_str()
				);

			return 0;
		}

        uint32_t count = m_resourceAnimation->getSequenceCount();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::getFrameDelay( uint32_t _frame ) const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation.getFrameDelay: '%s' not compiled resource"
				, m_name.c_str()
				);

			return 0.f;
		}

		float delay = m_resourceAnimation->getSequenceDelay( _frame );

        return delay;
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::setCurrentFrame( uint32_t _frame )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation.setCurrentFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( _frame >= sequenceCount )	
		{
			LOGGER_ERROR(m_serviceProvider)("Animation.setCurrentFrame: '%s' _frame(%d) >= sequenceCount(%d)"
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
		uint32_t firstFrame = 0;

		this->setCurrentFrame( firstFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Animation::_setLastFrame()
	{
		uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

		uint32_t lastFrame = sequenceCount - 1;

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
			LOGGER_ERROR(m_serviceProvider)("Animation._setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		float duration = m_resourceAnimation->getSequenceDuration();

		m_playIterator = this->getPlayCount();

		uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

		if( skipIterator > 0 )
		{
			m_playIterator -= skipIterator;
		}
				
		m_currentFrame = this->getFrame_( _timing, m_frameTiming );

		this->updateCurrentFrame_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Animation::_getTiming() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation._getTiming: '%s' not activate"
				, m_name.c_str()
				);

			return 0.f;
		}

		float timing = 0.f;

		for( uint32_t frame = 0; frame != m_currentFrame; ++frame )
		{
			float delay = m_resourceAnimation->getSequenceDelay( frame );

			timing += delay;
		}

		timing += m_frameTiming;

		return timing; 
	}
	//////////////////////////////////////////////////////////////////////////
	bool Animation::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
