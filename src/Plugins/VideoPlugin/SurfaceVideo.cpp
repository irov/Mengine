#include "SurfaceVideo.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/RenderUtils.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceVideo::SurfaceVideo()
        : m_time( 0.f )
        , m_needUpdateVideoBuffer( false )
        , m_updateFirstFrame( false )
        , m_size( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceVideo::~SurfaceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::setResourceVideo( const ResourcePtr & _resourceVideo )
    {
        if( m_resourceVideo == _resourceVideo )
        {
            return;
        }

        this->recompile( [this, _resourceVideo]() {m_resourceVideo = _resourceVideo; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SurfaceVideo::getResourceVideo() const
    {
        return m_resourceVideo;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::getWidth() const
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float width = (float)dataInfo->frameWidth;

        return width;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::getHeight() const
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float height = (float)dataInfo->frameHeight;

        return height;
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::updateSize_()
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float width = (float)dataInfo->frameWidth;
        float height = (float)dataInfo->frameHeight;

        m_size.x = width;
        m_size.y = height;
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::updateUV_()
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        const RenderTextureInterfacePtr & texture = m_textures[0];

        const RenderImageInterfacePtr & image = texture->getImage();

        uint32_t hwWidth = image->getHWWidth();
        uint32_t hwHeight = image->getHWHeight();

        float u = float( dataInfo->frameWidth ) / float( hwWidth );
        float v = float( dataInfo->frameHeight ) / float( hwHeight );

        m_uv.p0 = mt::vec2f( 0.f, 0.f );
        m_uv.p1 = mt::vec2f( u, 0.f );
        m_uv.p2 = mt::vec2f( u, v );
        m_uv.p3 = mt::vec2f( 0.f, v );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_activate()
    {
        this->updateVideoBuffer_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return false;
        }

        bool enabled = APPLICATION_SERVICE()
            ->getVideoEnable();

        if( enabled == false )
        {
            return false;
        }

        float totalTime = this->calcTotalTime( _context );

        m_needUpdateVideoBuffer = this->sync_( totalTime );

        this->updateVideoBuffer_();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceVideo, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceVideo->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
            );

            return false;
        }

        if( this->createDecoder_() == false )
        {
            LOGGER_ERROR( "%s can`t create video decoder '%s'"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
            );

            return false;
        }

        uint32_t channels;

        if( m_resourceVideo->isAlpha() == true )
        {
            channels = 4;
        }
        else
        {
            channels = 3;
        }

        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        RenderTextureInterfacePtr dynamicTexture = RENDERTEXTURE_SERVICE()
            ->createDynamicTexture( dataInfo->width, dataInfo->height, channels, 1, dataInfo->format, "SurfaceVideo" );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicTexture, false, "'%s' resource '%s' can`t create dynamic texture"
            , this->getName().c_str()
            , m_resourceVideo->getName().c_str()
        );

        m_textures[0] = dynamicTexture;

        this->invalidateMaterial();
        this->updateSize_();
        this->updateUV_();

        m_time = 0.f;
        m_needUpdateVideoBuffer = true;
        m_updateFirstFrame = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::createDecoder_()
    {
        VideoDecoderInterfacePtr videoDecoder = m_resourceVideo->createVideoDecoder( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( videoDecoder, false, "'%s' can't create video decoder"
            , this->getName().c_str()
        );

        m_videoDecoder = videoDecoder;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_release()
    {
        if( m_videoDecoder != nullptr )
        {
            m_resourceVideo->destroyVideoDecoder( m_videoDecoder );
            m_videoDecoder = nullptr;
        }

        m_resourceVideo->release();

        m_textures[0] = nullptr;

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getMaxSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceVideo::getOffset() const
    {
        return mt::vec2f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceVideo::getUVCount() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceVideo::getUV( uint32_t _index ) const
    {
        MENGINE_UNUSED( _index );

        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out )
    {
        MENGINE_UNUSED( _index );

        mt::uv4_quad_point( *_out, m_uv, _in );
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceVideo::getColor() const
    {
        return Color::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_stop( uint32_t _playId )
    {
        m_needUpdateVideoBuffer = false;

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_end( uint32_t _playId )
    {
        m_needUpdateVideoBuffer = false;

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_play( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;
        m_needUpdateVideoBuffer = true;
        m_updateFirstFrame = true;

        if( m_videoDecoder->rewind() == false )
        {
            LOGGER_ERROR( "video '%s' resource '%s' invalid rewind"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_restart( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _playId );

        m_time = 0.f;
        m_needUpdateVideoBuffer = true;
        m_updateFirstFrame = true;

        if( m_videoDecoder->rewind() == false )
        {
            LOGGER_ERROR( "video '%s' resource '%s' invalid rewind"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_pause( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_resume( uint32_t _playId, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationResume( _playId, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::updateVideoBuffer_() const
    {
        if( m_needUpdateVideoBuffer == true )
        {
            if( this->fillVideoBuffer_() == false )
            {
                LOGGER_ERROR( "video '%s' resource '%s' invalid fill video buffer"
                    , this->getName().c_str()
                    , m_resourceVideo->getName().c_str()
                );

                return;
            }

            m_needUpdateVideoBuffer = false;
        }
    }
    ////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::sync_( float _time )
    {
        m_time += _time;

        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        bool needUpdate = false;

        float frameTime = dataInfo->getFrameTime();
        float duration = dataInfo->duration;

        while( m_time >= frameTime || m_updateFirstFrame == true )
        {
            float pts;
            EVideoDecoderReadState state = m_videoDecoder->readNextFrame( m_time, pts );

            needUpdate = true;

            if( state == VDRS_SUCCESS )
            {
                //Empty
            }
            else if( state == VDRS_END_STREAM )
            {
                bool loop = this->isLoop();
                bool interrupt = this->isInterrupt();

                if( (loop == false && --m_playIterator == 0) || interrupt == true )
                {
                    m_videoDecoder->rewind();

                    this->end();

                    return false;
                }
                else
                {
                    float time = m_intervalBegin;

                    float intervalTime = duration * (m_playCount - m_playIterator);

                    if( m_intervalStart > intervalTime )
                    {
                        time += m_intervalStart - intervalTime;
                    }

                    if( time > frameTime )
                    {
                        time -= frameTime;
                    }

                    if( m_videoDecoder->seek( time ) == false )
                    {
                        LOGGER_ERROR( "video '%s' resource '%s' invalid seek to '%f'"
                            , this->getName().c_str()
                            , m_resourceVideo->getName().c_str()
                            , time
                        );
                    }
                }
            }
            else if( state == VDRS_FAILURE )
            {
                LOGGER_ERROR( "video '%s' error reading frame timing '%4.2f' total timing '%4.2f'"
                    , this->getName().c_str()
                    , _time
                    , m_time
                );

                needUpdate = false;

                break;
            }

            if( m_updateFirstFrame == false )
            {
                m_time -= frameTime;
            }

            m_updateFirstFrame = false;
        }

        return needUpdate;
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "vide '%s' resource '%s' not compile"
                , this->getName().c_str()
                , this->getResourceVideo()->getName().c_str()
            );

            return;
        }

        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float frameTime = dataInfo->getFrameTime();

        float frameRate = m_resourceVideo->getFrameRate();

        if( frameRate > 0.f )
        {
            frameTime = 1000.f / frameRate;
        }

        if( ::fabsf( m_time - _time ) < frameTime )
        {
            return;
        }

        float seek_time = _time;

        float duration = dataInfo->duration;

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

        if( m_videoDecoder->seek( seek_time ) == false )
        {
            LOGGER_ERROR( "video '%s' resource '%s' invalid seek to '%f'"
                , this->getName().c_str()
                , m_resourceVideo->getName().c_str()
                , seek_time
            );

            return;
        }

        m_needUpdateVideoBuffer = true;
    }
    ////////////////////////////////////////////////////////////////////
    float SurfaceVideo::_getTime() const
    {
        float timing = m_videoDecoder->tell();

        return timing;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::_getDuration() const
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        float duration = (float)dataInfo->duration;

        return duration;
    }

    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_setFirstFrame()
    {
        this->setTime( 0.f );
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_setLastFrame()
    {
        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        this->_setTime( dataInfo->duration );
    }
    ////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::fillVideoBuffer_() const
    {
        const RenderTextureInterfacePtr & texture = m_textures[0];

        const VideoCodecDataInfo * dataInfo = m_videoDecoder->getCodecDataInfo();

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        if( dataInfo->clamp == true )
        {
            rect.left = 0;
            rect.top = 0;
            rect.right = texture->getWidth();
            rect.bottom = texture->getHeight();
        }
        else
        {
            rect.left = 0;
            rect.top = 0;
            rect.right = image->getHWWidth();
            rect.bottom = image->getHWHeight();
        }

        size_t pitch = 0;
        void * lockRect = image->lock( &pitch, 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( lockRect, false, "'%s:%s' invalid lock texture %d:%d"
            , this->getName().c_str()
            , m_resourceVideo->getName().c_str()
            , rect.right
            , rect.bottom
        );

        m_videoDecoder->setPitch( pitch );

        size_t bufferSize = Helper::getImageMemorySize( image );
        size_t bytes = m_videoDecoder->decode( lockRect, bufferSize );

        image->unlock( 0, true );

        return bytes != 0;
    }
    ////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_interrupt( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        return true;
    }
    ////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceVideo::_updateMaterial() const
    {
        this->updateVideoBuffer_();

        RenderMaterialInterfacePtr material = this->makeTextureMaterial( 1, m_textureMasks, m_textures, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( material, nullptr, "surface %s invalid make material"
            , this->getName().c_str()
        );

        return material;
    }
    ////////////////////////////////////////////////////////////////////
}
