#include "ResourceVideo.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

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
#ifndef MENGINE_MASTER_RELEASE
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
#ifndef MENGINE_MASTER_RELEASE
        bool Engine_ForcePremultiplyAlpha = CONFIG_VALUE( "Engine", "ForcePremultiplyAlpha", false );

        if( Engine_ForcePremultiplyAlpha == true )
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
#ifndef MENGINE_MASTER_RELEASE
        if( m_forcePremultiply == true )
        {
            this->setPremultiply( false );

            m_forcePremultiply = false;
        }
#endif

        m_videoDecoderCacher.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder( const DocumentPtr & _doc )
    {
        const VideoDecoderInterfacePtr & cacheVideoDecoder = m_videoDecoderCacher.findCache( nullptr );

        if( cacheVideoDecoder != nullptr )
        {
            if( cacheVideoDecoder->rewind() == false )
            {
                LOGGER_ERROR( "group '%s' name '%s' invalid rewind cache video decoder for file '%s'"
                    , this->getGroupName().c_str()
                    , this->getName().c_str()
                    , this->getContent()->getFilePath().c_str()
                );

                return nullptr;
            }

            return cacheVideoDecoder;
        }

        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr videoStream = Helper::openInputStreamFile( fileGroup, filePath, true, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoStream, "group '%s' name '%s' can't open video file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        const ConstString & codecType = content->getCodecType();

        VideoDecoderInterfacePtr videoDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoDecoder, "group '%s' name '%s' can't create video decoder for file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , this->getContent()->getFilePath().c_str()
        );

        if( videoDecoder->prepareData( videoStream ) == false )
        {
            LOGGER_ERROR( "group '%s' name '%s' can't initialize video decoder for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , this->getContent()->getFilePath().c_str()
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
