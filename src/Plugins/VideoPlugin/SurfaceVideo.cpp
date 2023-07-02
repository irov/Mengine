#include "SurfaceVideo.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/RenderHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/ResourceHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceVideo::SurfaceVideo()
        : m_time( 0.f )
        , m_size( 0.f, 0.f )
        , m_needUpdateVideoBuffer( false )
        , m_updateFirstFrame( false )
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
    const ResourcePtr & SurfaceVideo::getResourceVideo() const
    {
        return m_resourceVideo;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::getWidth() const
    {
        return m_size.x;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::getHeight() const
    {
        return m_size.y;
    }
    ////////////////////////////////////////////////////////////////////
    void SurfaceVideo::updateUV_()
    {
        const RenderTextureInterfacePtr & texture = m_textures[0];

        const RenderImageInterfacePtr & image = texture->getImage();

        float hwWidthInv = image->getHWWidthInv();
        float hwHeightInv = image->getHWHeightInv();

        float u = m_size.x * hwWidthInv;
        float v = m_size.y * hwHeightInv;

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
    void SurfaceVideo::_update( const UpdateContext * _context )
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

        m_needUpdateVideoBuffer = this->sync_( totalTime );

        this->updateVideoBuffer_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceVideo, "'%s' resource is nullptr"
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

        this->createDecoder_();

        uint32_t flags = this->getDecodeFlags_();

        VideoSurfaceDimension surfaceDimension;
        m_decoderVideo->getSurfaceDimension( flags, &surfaceDimension );

        RenderTextureInterfacePtr dynamicTexture = RENDERTEXTURE_SERVICE()
            ->createTexture( 1, surfaceDimension.width, surfaceDimension.height, surfaceDimension.format, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicTexture, "'%s' resource '%s' can`t create dynamic texture"
            , this->getName().c_str()
            , m_resourceVideo->getName().c_str()
        );

        m_size.x = (float)surfaceDimension.frameWidth;
        m_size.y = (float)surfaceDimension.frameHeight;

        m_textures[0] = dynamicTexture;

        this->invalidateMaterial();
        this->updateUV_();

        m_time = 0.f;
        m_needUpdateVideoBuffer = true;
        m_updateFirstFrame = true;

        LOGGER_INFO( "video", "compile '%s' file '%s' size %ux%u duration [%.2f] %s %s"
            , this->getName().c_str()
            , Helper::getResourceFilePath( this->getResourceVideo() ).c_str()
            , surfaceDimension.frameWidth
            , surfaceDimension.frameHeight
            , m_resourceVideo->getDuration()
            , m_resourceVideo->isAlpha() == true ? "[alpha]" : ""
            , m_resourceVideo->isPremultiply() == true ? "[premultiply]" : ""
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::createDecoder_()
    {
        VideoDecoderInterfacePtr videoDecoder = m_resourceVideo->createVideoDecoder( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( videoDecoder, "'%s' can't create video decoder"
            , this->getName().c_str()
        );

        m_decoderVideo = videoDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceVideo::_release()
    {
        LOGGER_INFO( "video", "release '%s' file '%s'"
            , this->getName().c_str()
            , Helper::getResourceFilePath( this->getResourceVideo() ).c_str()
        );

        if( m_decoderVideo != nullptr )
        {
            m_resourceVideo->destroyVideoDecoder( m_decoderVideo );
            m_decoderVideo = nullptr;
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
    void SurfaceVideo::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_UNUSED( _index );

        mt::uv4_quad_point( _out, m_uv, _in );
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

        //m_time = 0.f;
        m_needUpdateVideoBuffer = true;
        //m_updateFirstFrame = true;

        //if( m_decoderVideo->rewind() == false )
        //{
        //    LOGGER_ERROR( "video '%s' resource '%s' invalid rewind"
        //        , this->getName().c_str()
        //        , m_resourceVideo->getName().c_str()
        //    );

        //    return false;
        //}

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

        if( m_decoderVideo->rewind() == false )
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

        bool needUpdate = false;

        float frameRate = m_resourceVideo->getFrameRate();
        float duration = m_resourceVideo->getDuration();

        float frameTime = 1000.f / frameRate;

        while( m_time >= frameTime || m_updateFirstFrame == true )
        {
            float pts;
            EVideoDecoderReadState state = m_decoderVideo->readNextFrame( m_time, &pts );

            needUpdate = true;

            if( state == VDRS_SUCCESS )
            {
                //Empty
            }
            else if( state == VDRS_END_STREAM )
            {
                bool loop = this->calcTotalLoop();
                bool interrupt = this->isInterrupt();

                if( (loop == false && --m_playIterator == 0) || interrupt == true )
                {
                    if( m_decoderVideo->rewind() == false )
                    {
                        return false;
                    }

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

                    if( m_decoderVideo->seek( time ) == false )
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
            LOGGER_ERROR( "video '%s' resource '%s' not compile"
                , this->getName().c_str()
                , this->getResourceVideo()->getName().c_str()
            );

            return;
        }

        float frameRate = m_resourceVideo->getFrameRate();
        float frameTime = 1000.f / frameRate;

        if( MENGINE_FABSF( m_time - _time ) < frameTime )
        {
            return;
        }

        float seek_time = _time;

        float duration = m_resourceVideo->getDuration();

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

        if( m_decoderVideo->seek( seek_time ) == false )
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
        float timing = m_decoderVideo->tell();

        return timing;
    }
    //////////////////////////////////////////////////////////////////////////
    float SurfaceVideo::_getDuration() const
    {
        float duration = m_resourceVideo->getDuration();

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
        float duration = m_resourceVideo->getDuration();

        this->_setTime( duration );
    }
    ////////////////////////////////////////////////////////////////////
    bool SurfaceVideo::fillVideoBuffer_() const
    {
        const RenderTextureInterfacePtr & texture = m_textures[0];

        const VideoCodecDataInfo * dataInfo = m_decoderVideo->getCodecDataInfo();

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

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, false );

        MENGINE_ASSERTION_MEMORY_PANIC( locked, "video '%s' invalid lock image"
            , this->getName().c_str()
        );

        size_t pitch = 0;
        void * lockRect = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( lockRect, "surface '%s' resource '%s' invalid lock texture [%u:%u]"
            , this->getName().c_str()
            , m_resourceVideo->getName().c_str()
            , rect.right
            , rect.bottom
        );

        size_t bufferSize = pitch * (rect.bottom - rect.top);

        EPixelFormat hwPixelFormat = image->getHWPixelFormat();

        VideoDecoderData data;
        data.buffer = lockRect;
        data.size = bufferSize;
        data.pitch = pitch;
        data.format = hwPixelFormat;
        data.flags = this->getDecodeFlags_();

        size_t bytes = m_decoderVideo->decode( &data );

        image->unlock( locked, 0, true );

        if( bytes == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceVideo::getDecodeFlags_() const
    {
        uint32_t flags = DF_VIDEO_NONE;

        if( m_resourceVideo->isAlpha() == true )
        {
            flags |= DF_VIDEO_SEPARATE_HORIZONTAL_ALPHA;
        }

        if( m_resourceVideo->isPremultiply() == true )
        {
            flags |= DF_VIDEO_PREMULTIPLY_ALPHA;
        }

        return flags;
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

        bool premultiply = m_resourceVideo->isPremultiply();

        RenderMaterialInterfacePtr material = this->makeTextureMaterial( 1, m_textures, premultiply, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "surface '%s' invalid make material"
            , this->getName().c_str()
        );

        return material;
    }
    ////////////////////////////////////////////////////////////////////
}
