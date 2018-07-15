#include "SurfaceImageSequence.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/TimelineInterface.h"

#include "Kernel/ResourceImage.h"

#include "ResourceAnimation.h"

#include "Logger/Logger.h"

#include <math.h>

namespace Mengine
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
	bool SurfaceImageSequence::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
            bool invalidate = this->isInvalidateMaterial();

			return invalidate;
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

                EVENTABLE_METHOD( this, EVENT_SURFACE_IMAGESEQUENCE_FRAME_END )
                    ->onSurfaceImageSequenceFrameEnd( m_currentFrame );
				
				++m_currentFrame;

                EVENTABLE_METHOD( this, EVENT_SURFACE_IMAGESEQUENCE_FRAME_TICK )
                    ->onSurfaceImageSequenceFrameTick( m_currentFrame, frameCount );

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
						float adaptFrameTiming = this->getAdaptTime(m_frameTiming);

						float newFrameTiming;
                        m_currentFrame = this->getFrame_( adaptFrameTiming, newFrameTiming );
						m_frameTiming = newFrameTiming;
                    }

                    //lastFrame = m_currentFrame;
                }

                frameDelay = m_resourceAnimation->getSequenceDelay( m_currentFrame );                
            }
        }

        if( lastFrame == m_currentFrame )
        {
            return false;
        }
		
        this->invalidateMaterial();
		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_compile()
	{
		if( m_resourceAnimation == nullptr )
		{
			LOGGER_ERROR("SurfaceImageSequence::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

        if( m_resourceAnimation.compile() == false )
        {
            LOGGER_ERROR("Animation::_compile: '%s' resource '%s' is not compile"
                , m_name.c_str()
                , m_resourceAnimation->getName().c_str()
                );

            return false;
        }

		uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

		if( m_currentFrame >= sequenceCount )
		{
			LOGGER_ERROR("Animation::_compile: '%s' m_frame(%d) >= sequenceCount(%d)"
				, m_name.c_str()
				, m_currentFrame
				, sequenceCount
				);

			return false;
		}

		m_materials.resize(sequenceCount);

		for (uint32_t frameId = 0; frameId != sequenceCount; ++frameId)
		{
			const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource(frameId);

			if (resourceImage->compile() == false)
			{
				LOGGER_ERROR("SurfaceImageSequence::_updateMaterial '%s' invalid compile %d frame"
					, this->getName().c_str()
					, m_currentFrame
					);

				return false;
			}

			RenderMaterialInterfacePtr material = this->makeImageMaterial(resourceImage, false);

			if (material == nullptr)
			{
				LOGGER_ERROR("SurfaceImageSequence::updateMaterial_ %s resource %s m_material is NULL"
					, this->getName().c_str()
					, resourceImage->getName().c_str()
					);

				return false;
			}

			m_materials[frameId] = material;
		}

		this->invalidateMaterial();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_release()
	{
		m_materials.clear();

        m_resourceAnimation.release();

		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_play( uint32_t _enumerator, float _time )
	{
        (void)_enumerator;
        (void)_time;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceImageSequence::_restart( uint32_t _enumerator, float _time )
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
	void SurfaceImageSequence::_resume( uint32_t _enumerator, float _time )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_stop( uint32_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}
        
        EVENTABLE_METHOD( this, EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::_end( uint32_t _enumerator )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("Animation: '%s' end not activate"
				, getName().c_str()
				);

			return;
		}
        
        EVENTABLE_METHOD( this, EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
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
        uint32_t count = m_resourceAnimation->getSequenceCount();

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	float SurfaceImageSequence::getFrameDelay( uint32_t _frame ) const
	{
		float delay = m_resourceAnimation->getSequenceDelay( _frame );

        return delay;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceImageSequence::setCurrentFrame( uint32_t _frame )
	{
		m_currentFrame = _frame;
		m_frameTiming = 0.f;

#ifndef NDEBUG
        if( this->isCompile() == true )
        {
            uint32_t sequenceCount = m_resourceAnimation->getSequenceCount();

            if( _frame >= sequenceCount )
            {
                LOGGER_ERROR("Animation.setCurrentFrame: '%s' _frame(%d) >= sequenceCount(%d)"
                    , m_name.c_str()
                    , _frame
                    , sequenceCount
                    );

                return;
            }
        }
#endif

		this->invalidateMaterial();
	}
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getMaxSize() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getMaxSize: '%s' not compile"
                , this->getName().c_str()
                );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        const mt::vec2f & maxSize = resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getSize() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getSize: '%s' not compile"
                , this->getName().c_str()
                );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        const mt::vec2f & size = resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getOffset() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getOffset: '%s' not compile"
                , this->getName().c_str()
                );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        const mt::vec2f & offset = resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImageSequence::getUVCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getUVCount: '%s' not compile"
                , this->getName().c_str()
                );

            return 0;
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        const RenderTextureInterfacePtr & texture = resourceImage->getTexture();

        if( texture == nullptr )
        {
            return 0;
        }

        const RenderTextureInterfacePtr & textureAlpha = resourceImage->getTextureAlpha();

        if( textureAlpha == nullptr )
        {
            return 1;
        }

        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceImageSequence::getUV( uint32_t _index ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getUV: '%s' not compile"
                , this->getName().c_str()
                );

            return mt::uv4f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        switch( _index )
        {
        case 0:
            {
                const mt::uv4f & uv = resourceImage->getUVImage();

                return uv;
            } break;
        case 1:
            {
                const mt::uv4f & uv = resourceImage->getUVAlpha();

                return uv;
            } break;
        default:
            {
            }break;
        }

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "SurfaceImageSequence::correctUV: '%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        switch( _index )
        {
        case 0:
            {
                resourceImage->correctUVImage( _out, _in );
            } break;
        case 1:
            {
                resourceImage->correctUVAlpha( _out, _in );
            } break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceImageSequence::getColor() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR("SurfaceImageSequence.getColor: '%s' not compile"
                , this->getName().c_str()
                );

            return ColourValue::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceAnimation->getSequenceResource( m_currentFrame );

        const ColourValue & color = resourceImage->getColor();

        return color;
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
	void SurfaceImageSequence::_setTime( float _timing )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("Animation._setTiming: '%s' not activate"
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
	float SurfaceImageSequence::_getTime() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR("Animation._getTiming: '%s' not activate"
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
		const RenderMaterialInterfacePtr & material = m_materials[m_currentFrame];

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
}
