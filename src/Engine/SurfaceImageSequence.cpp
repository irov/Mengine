#include "SurfaceImageSequence.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "ResourceImageSequence.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EventableHelper.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceImageSequence::SurfaceImageSequence()
        : m_frameTime( 0.f )
        , m_currentFrame( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceImageSequence::~SurfaceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::setResourceImageSequence( const ResourceImageSequencePtr & _resourceImageSequence )
    {
        if( m_resourceImageSequence == _resourceImageSequence )
        {
            return;
        }

        this->recompile( [this, _resourceImageSequence]()
        {
            m_resourceImageSequence = _resourceImageSequence;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImageSequencePtr & SurfaceImageSequence::getResourceImageSequence() const
    {
        return m_resourceImageSequence;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            bool invalidate = this->isInvalidateMaterial();

            return invalidate;
        }

        float totalTiming = this->calcTotalTime( _context );

        m_frameTime += totalTiming;

        uint32_t frameCount = m_resourceImageSequence->getSequenceCount();

        float frameDelay = m_resourceImageSequence->getSequenceDelay( m_currentFrame );

        uint32_t lastFrame = m_currentFrame;

        if( m_currentFrame != frameCount )
        {
            while( m_frameTime >= frameDelay )
            {
                m_frameTime -= frameDelay;

                EVENTABLE_METHOD( EVENT_SURFACE_IMAGESEQUENCE_FRAME_END )
                    ->onSurfaceImageSequenceFrameEnd( m_currentFrame );

                ++m_currentFrame;

                EVENTABLE_METHOD( EVENT_SURFACE_IMAGESEQUENCE_FRAME_TICK )
                    ->onSurfaceImageSequenceFrameTick( m_currentFrame, frameCount );

                if( m_currentFrame == frameCount )
                {
                    bool loop = this->isLoop();
                    bool interrupt = this->isInterrupt();

                    if( (loop == false && --m_playIterator == 0) || interrupt == true )
                    {
                        m_currentFrame = frameCount - 1;
                        m_frameTime = 0.f;

                        lastFrame = m_currentFrame;

                        this->end();

                        break;
                    }
                    else
                    {
                        float adaptFrameTiming = this->getAdaptTime( m_frameTime );

                        float newFrameTiming;
                        m_currentFrame = this->getFrame_( adaptFrameTiming, newFrameTiming );
                        m_frameTime = newFrameTiming;
                    }
                }

                frameDelay = m_resourceImageSequence->getSequenceDelay( m_currentFrame );
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
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageSequence, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceImageSequence->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceImageSequence->getName().c_str()
            );

            return false;
        }

        uint32_t sequenceCount = m_resourceImageSequence->getSequenceCount();

        MENGINE_ASSERTION_RETURN( m_currentFrame < sequenceCount, false, "'%s' current frame %d < %d"
            , this->getName().c_str()
            , m_currentFrame
            , sequenceCount
        );

        m_materials.resize( sequenceCount );

        for( uint32_t frameId = 0; frameId != sequenceCount; ++frameId )
        {
            const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( frameId );

            if( resourceImage->compile() == false )
            {
                LOGGER_ERROR( "'%s' invalid compile %d frame"
                    , this->getName().c_str()
                    , m_currentFrame
                );

                return false;
            }

            RenderMaterialInterfacePtr material = this->makeImageMaterial( resourceImage, false, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( material, false, "'%s' resource '%s' m_material is NULL"
                , this->getName().c_str()
                , resourceImage->getName().c_str()
            );

            m_materials[frameId] = material;
        }

        this->invalidateMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_release()
    {
        m_materials.clear();

        m_resourceImageSequence->release();

        m_play = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_play( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_restart( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_pause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_resume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_stop( uint32_t _enumerator )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' stop not activate"
                , getName().c_str()
            );

            return false;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_end( uint32_t _enumerator )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' end not activate"
                , getName().c_str()
            );

            return;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImageSequence::getFrame_( float _time, float & _deltaTime ) const
    {
        if( _time <= 0.f )
        {
            _deltaTime = _time;

            return 0;
        }

        float duration = m_resourceImageSequence->getSequenceDuration();

        if( _time >= duration )
        {
            _time -= ::floorf( _time / duration ) * duration;

            if( ::fabsf( _time ) < 0.0001f )
            {
                _deltaTime = 0.f;

                return 0;
            }
        }

        uint32_t count = m_resourceImageSequence->getSequenceCount();

        for( uint32_t frame = 0; frame != count; ++frame )
        {
            float delay = m_resourceImageSequence->getSequenceDelay( frame );

            _time -= delay;

            if( _time < 0.f )
            {
                _deltaTime = _time + delay;

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
        uint32_t count = m_resourceImageSequence->getSequenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceImageSequence::getFrameDelay( uint32_t _frame ) const
    {
        float delay = m_resourceImageSequence->getSequenceDelay( _frame );

        return delay;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::setCurrentFrame( uint32_t _frame )
    {
        m_currentFrame = _frame;
        m_frameTime = 0.f;

#ifdef MENGINE_DEBUG
        if( this->isCompile() == true )
        {
            uint32_t sequenceCount = m_resourceImageSequence->getSequenceCount();

            if( _frame >= sequenceCount )
            {
                LOGGER_ERROR( "'%s' _frame(%d) >= sequenceCount(%d)"
                    , this->getName().c_str()
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
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & maxSize = resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getSize() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & size = resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getOffset() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return mt::vec2f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & offset = resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImageSequence::getUVCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return 0;
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

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
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return mt::uv4f::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        switch( _index )
        {
        case 0:
            {
                const mt::uv4f & uv = resourceImage->getUVTextureImage();

                return uv;
            } break;
        case 1:
            {
                const mt::uv4f & uv = resourceImage->getUVTextureAlpha();

                return uv;
            } break;
        default:
            {
            }break;
        }

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return;
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        switch( _index )
        {
        case 0:
            {
                resourceImage->correctUVImage( _in, _out );
            } break;
        case 1:
            {
                resourceImage->correctUVAlpha( _in, _out );
            } break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceImageSequence::getColor() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return Color::identity();
        }

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const Color & color = resourceImage->getColor();

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
        uint32_t sequenceCount = m_resourceImageSequence->getSequenceCount();

        uint32_t lastFrame = sequenceCount - 1;

        this->setCurrentFrame( lastFrame );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_setTime( float _timing )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not activate"
                , this->getName().c_str()
            );

            return;
        }

        float duration = m_resourceImageSequence->getSequenceDuration();

        m_playIterator = this->getPlayCount();

        uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

        if( skipIterator > 0 )
        {
            m_playIterator -= skipIterator;
        }

        m_currentFrame = this->getFrame_( _timing, m_frameTime );

        this->invalidateMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceImageSequence::_getTime() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not activate"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float timing = 0.f;

        for( uint32_t frame = 0; frame != m_currentFrame; ++frame )
        {
            float delay = m_resourceImageSequence->getSequenceDelay( frame );

            timing += delay;
        }

        timing += m_frameTime;

        return timing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_interrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        //Empty

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
