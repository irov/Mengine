#	include "SurfaceImageSequence.h"

#   include "Interface/TimelineInterface.h"

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
	SurfaceImageSequence::SurfaceImageSequence()
		: m_frameTiming(0.f)
		, m_currentFrame(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SurfaceImageSequence::~SurfaceImageSequence()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::setResourceAnimation( const ResourceAnimationPtr & _resourceAnimation )
	{
		if( m_resourceAnimation == _resourceAnimation ) 
		{
			return;
		}

		m_resourceAnimation = _resourceAnimation;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceAnimationPtr & SurfaceImageSequence::getResourceAnimation() const
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
	void SurfaceImageSequence::_setEventListener( const pybind::dict & _listener )
	{
		Surface::_setEventListener( _listener );

		this->registerEvent( EVENT_FRAME_END, ("onFrameEnd"), _listener );
		this->registerEvent( EVENT_FRAME_TICK, ("onFrameTick"), _listener );
		this->registerEvent( EVENT_ANIMATION_END, ("onAnimationEnd"), _listener );

		this->registerEvent( EVENT_ANIMATABLE_END, ("onAnimatableEnd"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_update( float _current, float _timing )
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
			this->invalidateMaterial();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_compile()
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

		uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( m_currentFrame >= sequenceCount )
		{
			LOGGER_ERROR( m_serviceProvider )("Animation::_compile: '%s' m_frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, m_currentFrame
				, sequenceCount
				);

			return false;
		}

		this->invalidateMaterial();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_release()
	{
        m_resourceAnimation.release();

		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_play( float _time )
	{
        (void)_time;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_restart( float _time, uint32_t _enumerator )
	{
        (void)_time;
        (void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_stop( uint32_t _enumerator )
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
	void SurfaceImageSequence::_end( uint32_t _enumerator )
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
	uint32_t SurfaceImageSequence::getFrame_( float _timing, float & _delthaTiming ) const
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
	uint32_t SurfaceImageSequence::getCurrentFrame() const
	{
		return m_currentFrame;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t SurfaceImageSequence::getFrameCount() const
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
	float SurfaceImageSequence::getFrameDelay( uint32_t _frame ) const
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
	void SurfaceImageSequence::setCurrentFrame( uint32_t _frame )
	{
		m_currentFrame = _frame;
		m_frameTiming = 0.f;

		if( this->isCompile() == false )
		{
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

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_setFirstFrame()
	{
		uint32_t firstFrame = 0;

		this->setCurrentFrame( firstFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_setLastFrame()
	{
		uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

		uint32_t lastFrame = sequenceCount - 1;

		this->setCurrentFrame( lastFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_setTiming( float _timing )
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

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceImageSequence::_getTiming() const
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
	bool SurfaceImageSequence::_interrupt( uint32_t _enumerator )
	{
        (void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceImageSequence::_updateMaterial() const
	{
		const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

		if( resourceImage->compile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceImageSequence::_updateMaterial '%s' invalid compile %d frame"
				, this->getName().c_str()
				, m_currentFrame
				);

			return nullptr;
		}
		
		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_serviceProvider, resourceImage, false );

		if( material == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceImageSequence::updateMaterial_ %s resource %s m_material is NULL"
				, this->getName().c_str()
				, resourceImage->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
}
