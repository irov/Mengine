#include "SurfaceMockupVideo.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/EventableHelper.h"

#include "Config/StdMath.h"

//ToDo https://github.com/irov/Mengine/issues/94

#ifndef MENGINE_SURFACE_MOCKUP_VIDEO_FRAMERATE
#define MENGINE_SURFACE_MOCKUP_VIDEO_FRAMERATE (24.f)
#endif

#ifndef MENGINE_SURFACE_MOCKUP_VIDEO_DURATION
#define MENGINE_SURFACE_MOCKUP_VIDEO_DURATION (5000.f)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceMockupVideo::SurfaceMockupVideo()
        : m_time( 0.f )
        , m_size( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceMockupVideo::~SurfaceMockupVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::setResourceVideo( const ResourcePtr & _resourceVideo )
    {
        if( m_resourceVideo == _resourceVideo )
        {
            return;
        }

        this->recompile( [this, &_resourceVideo]()
        {
            m_resourceVideo = _resourceVideo;

            if( m_resourceVideo == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SurfaceMockupVideo::getResourceVideo() const
    {
        return m_resourceVideo;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceMockupVideo::getWidth() const
    {
        return m_size.x;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceMockupVideo::getHeight() const
    {
        return m_size.y;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        bool enabled = APPLICATION_SERVICE()
            ->getVideoEnable();

        if( enabled == false )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        this->sync_( totalTime );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceMockupVideo::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceVideo, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceMockupVideo::getMaxSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceMockupVideo::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceMockupVideo::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceMockupVideo::getUVCount() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceMockupVideo::getUV( uint32_t _index ) const
    {
        MENGINE_UNUSED( _index );

        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_UNUSED( _index );

        mt::uv4_quad_point( _out, m_uv, _in );
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceMockupVideo::getColor() const
    {
        return Color::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceMockupVideo::_stop( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_end( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceMockupVideo::_play( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceMockupVideo::_restart( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _playId );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_pause( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_resume( uint32_t _playId, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationResume( _playId, _time );
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::sync_( float _time )
    {
        m_time += _time;

        float duration = MENGINE_SURFACE_MOCKUP_VIDEO_DURATION;

        while( m_time >= duration )
        {
            m_time -= duration;

            bool loop = this->calcTotalLoop();
            bool interrupt = this->isInterrupt();

            if( (loop == false && --m_playIterator == 0) || interrupt == true )
            {
                this->end();

                return;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "video '%s' resource '%s' not compile"
                , this->getName().c_str()
                , this->getResourceVideo()->getName().c_str()
            );

            return;
        }

        float frameRate = MENGINE_SURFACE_MOCKUP_VIDEO_FRAMERATE;
        
        float frameTime = 1000.f / frameRate;

        if( MENGINE_FABSF( m_time - _time ) < frameTime )
        {
            return;
        }

        float duration = MENGINE_SURFACE_MOCKUP_VIDEO_DURATION;

        m_playIterator = this->getPlayCount();

        uint32_t skipIterator = (uint32_t)((m_intervalStart / duration) + 0.5f);

        if( skipIterator > 0 )
        {
            m_playIterator -= skipIterator;
        }

        m_time = _time;

        while( m_time > duration )
        {
            m_time -= duration;
        }
    }
    ////////////////////////////////////////////////////////////////////
    float SurfaceMockupVideo::_getTime() const
    {
        return m_time;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceMockupVideo::_getDuration() const
    {
        float duration = MENGINE_SURFACE_MOCKUP_VIDEO_DURATION;

        return duration;
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_setFirstFrame()
    {
        this->setTime( 0.f );
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceMockupVideo::_setLastFrame()
    {
        float duration = MENGINE_SURFACE_MOCKUP_VIDEO_DURATION;

        this->_setTime( duration );
    }
    ////////////////////////////////////////////////////////////////////
    bool SurfaceMockupVideo::_interrupt( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        return true;
    }
    ////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceMockupVideo::_updateMaterial() const
    {
        const RenderTextureInterfacePtr & nullTexture = RENDER_SERVICE()
            ->getNullTexture();

        RenderTextureInterfacePtr textures[1] = {nullTexture};

        RenderMaterialInterfacePtr material = this->makeTextureMaterial( 1, textures, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "surface '%s' invalid make material"
            , this->getName().c_str()
        );

        return material;
    }
    ////////////////////////////////////////////////////////////////////
}
