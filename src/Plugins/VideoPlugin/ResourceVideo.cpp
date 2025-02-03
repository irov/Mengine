#include "ResourceVideo.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceVideo::ResourceVideo()
        : m_frameRate( 0.f )
        , m_duration( 0.f )
        , m_resize( 0.f )
        , m_alpha( false )
        , m_premultiply( false )
        , m_noSeek( false )
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        , m_forcePremultiply( false )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceVideo::~ResourceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setFrameRate( float _frameRate )
    {
        m_frameRate = _frameRate;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceVideo::getFrameRate() const
    {
        return m_frameRate;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setDuration( float _duration )
    {
        m_duration = _duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceVideo::getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setResize( float _resize )
    {
        m_resize = _resize;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceVideo::getResize() const
    {
        return m_resize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_compile()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool VideoPlugin_ForcePremultiplyAlpha = CONFIG_VALUE_BOOLEAN( "VideoPlugin", "ForcePremultiplyAlpha", false );

        if( VideoPlugin_ForcePremultiplyAlpha == true )
        {
            bool alpha = this->isAlpha();
            bool premultiply = this->isPremultiply();

            if( premultiply == false && alpha == true )
            {
                this->setPremultiply( true );

                m_forcePremultiply = true;
            }
        }
#endif

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::_release()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( m_forcePremultiply == true )
        {
            this->setPremultiply( false );

            m_forcePremultiply = false;
        }
#endif

        m_videoDecoderCacher.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder( const DocumentInterfacePtr & _doc )
    {
        const VideoDecoderInterfacePtr & cacheVideoDecoder = m_videoDecoderCacher.findCache( nullptr );

        if( cacheVideoDecoder != nullptr )
        {
            if( cacheVideoDecoder->rewind() == false )
            {
                LOGGER_ERROR( "resource video '%s' group '%s' invalid rewind cache video decoder for file '%s'"
                    , this->getName().c_str()
                    , this->getGroupName().c_str()                    
                    , Helper::getContentFullPath( this->getContent() ).c_str()
                );

                return nullptr;
            }

            return cacheVideoDecoder;
        }

        const ContentInterfacePtr & content = this->getContent();

        InputStreamInterfacePtr videoStream = content->openInputStreamFile( true, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoStream, "group '%s' name '%s' can't open video file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        const ConstString & codecType = content->getCodecType();

        VideoDecoderInterfacePtr videoDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoDecoder, "group '%s' name '%s' can't create video decoder for file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        if( videoDecoder->prepareData( videoStream ) == false )
        {
            LOGGER_ERROR( "resource video '%s' group '%s' can't initialize video decoder for file '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()                
                , Helper::getContentFullPath( this->getContent() ).c_str()
            );

            return nullptr;
        }

        m_videoDecoderCacher.addCache( videoDecoder );

        return videoDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder )
    {
        m_videoDecoderCacher.removeCache( _decoder );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::_cache()
    {
        m_videoDecoderCacher.lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::_uncache()
    {
        m_videoDecoderCacher.unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setAlpha( bool _alpha )
    {
        m_alpha = _alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::isAlpha() const
    {
        return m_alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setPremultiply( bool _premultiply )
    {
        m_premultiply = _premultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::isPremultiply() const
    {
        return m_premultiply;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setNoSeek( bool _noSeek )
    {
        m_noSeek = _noSeek;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::isNoSeek() const
    {
        return m_noSeek;
    }
    //////////////////////////////////////////////////////////////////////////
}
