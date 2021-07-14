#include "SurfaceImageSequence.h"

#include "Interface/TimelineServiceInterface.h"

#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

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

        this->recompile( [this, &_resourceImageSequence]()
        {
            m_resourceImageSequence = _resourceImageSequence;

            if( m_resourceImageSequence == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImageSequencePtr & SurfaceImageSequence::getResourceImageSequence() const
    {
        return m_resourceImageSequence;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        m_frameTime += totalTime;

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
                        float adaptFrameTime = this->getAdaptTime( m_frameTime );

                        float newFrameTime;
                        m_currentFrame = this->getFrame_( adaptFrameTime, &newFrameTime );
                        m_frameTime = newFrameTime;
                    }
                }

                frameDelay = m_resourceImageSequence->getSequenceDelay( m_currentFrame );
            }
        }

        if( lastFrame == m_currentFrame )
        {
            return;
        }

        this->invalidateMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageSequence, "'%s' resource is nullptr"
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

        MENGINE_ASSERTION_FATAL( m_currentFrame < sequenceCount, "'%s' current frame %u < %u"
            , this->getName().c_str()
            , m_currentFrame
            , sequenceCount
        );

        m_materials.resize( sequenceCount );

        for( uint32_t frameId = 0; frameId != sequenceCount; ++frameId )
        {
            const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( frameId );

            RenderMaterialInterfacePtr material = this->makeImageMaterial( resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' resource '%s' m_material is nullptr"
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
    bool SurfaceImageSequence::_play( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_restart( UniqueId _enumerator, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _enumerator );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImageSequence::_pause( UniqueId _enumerator )
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
    bool SurfaceImageSequence::_stop( UniqueId _enumerator )
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
    void SurfaceImageSequence::_end( UniqueId _enumerator )
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
    uint32_t SurfaceImageSequence::getFrame_( float _time, float * const _deltaTime ) const
    {
        if( _time <= 0.f )
        {
            *_deltaTime = _time;

            return 0;
        }

        float duration = m_resourceImageSequence->getSequenceDuration();

        if( _time >= duration )
        {
            _time -= MT_floorf( _time / duration ) * duration;

            if( MT_fabsf( _time ) < 0.0001f )
            {
                *_deltaTime = 0.f;

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
                *_deltaTime = _time + delay;

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
                LOGGER_ERROR( "invalid '%s' set frame %u >= %u"
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
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & maxSize = resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getSize() const
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & size = resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImageSequence::getOffset() const
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

        const ResourceImagePtr & resourceImage = m_resourceImageSequence->getSequenceResource( m_currentFrame );

        const mt::vec2f & offset = resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImageSequence::getUVCount() const
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

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
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

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
    void SurfaceImageSequence::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

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
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

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
    void SurfaceImageSequence::_setTime( float _time )
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

        float duration = m_resourceImageSequence->getSequenceDuration();

        m_playIterator = this->getPlayCount();

        uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

        if( skipIterator > 0 )
        {
            m_playIterator -= skipIterator;
        }

        m_currentFrame = this->getFrame_( _time, &m_frameTime );

        this->invalidateMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceImageSequence::_getTime() const
    {
        MENGINE_ASSERTION_FATAL( this->isCompile() == true, "'%s' not compile"
            , this->getName().c_str()
        );

        float time = 0.f;

        for( uint32_t frame = 0; frame != m_currentFrame; ++frame )
        {
            float delay = m_resourceImageSequence->getSequenceDelay( frame );

            time += delay;
        }

        time += m_frameTime;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImageSequence::_interrupt( UniqueId _enumerator )
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
