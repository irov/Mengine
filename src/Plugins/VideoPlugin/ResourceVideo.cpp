#include "ResourceVideo.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceVideo::ResourceVideo()
        : m_alpha( false )
        , m_noSeek( false )
        , m_frameRate( 0.f )
        , m_duration( 0.f )
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
    bool ResourceVideo::_compile()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::_release()
    {
        m_videoDecoderCacher.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder( const Char * _doc ) const
    {
        const VideoDecoderInterfacePtr & cacheVideoDecoder = m_videoDecoderCacher.findCache();

        if( cacheVideoDecoder != nullptr )
        {
            cacheVideoDecoder->rewind();

            return cacheVideoDecoder;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();
        const FilePath & filePath = this->getFilePath();

        InputStreamInterfacePtr videoStream = Helper::openInputStreamFile( fileGroup, filePath, true, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoStream, nullptr, "group '%s' name '%s' can't open video file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        const ConstString & codecType = this->getCodecType();

        VideoDecoderInterfacePtr videoDecoder = CODEC_SERVICE()
            ->createDecoderT<VideoDecoderInterfacePtr>( codecType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( videoDecoder, nullptr, "group '%s' name '%s' can't create video decoder for file '%s'"
            , this->getGroupName().c_str()
            , this->getName().c_str()
            , this->getFilePath().c_str()
        );

        VideoCodecOptions videoCodecOptions;

        if( m_alpha == true )
        {
            videoCodecOptions.pixelFormat = PF_A8R8G8B8;
        }
        else
        {
            if( RENDER_SYSTEM()
                ->supportTextureFormat( PF_R8G8B8 ) == true )
            {
                videoCodecOptions.pixelFormat = PF_R8G8B8;
            }
            else
            {
                videoCodecOptions.pixelFormat = PF_X8R8G8B8;
            }
        }

        videoCodecOptions.duration = m_duration;
        videoCodecOptions.fps = m_frameRate;
        videoCodecOptions.alpha = m_alpha;

        videoCodecOptions.mock = HAS_OPTION( "novideo" );
        videoCodecOptions.noSeek = m_noSeek;

        if( videoDecoder->setOptions( &videoCodecOptions ) == false )
        {
            LOGGER_ERROR( "group '%s' name '%s' can't setup options for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return nullptr;
        }

        if( videoDecoder->prepareData( videoStream ) == false )
        {
            LOGGER_ERROR( "group '%s' name '%s' can't initialize video decoder for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return nullptr;
        }

        m_videoDecoderCacher.addCache( videoDecoder );

        return videoDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder ) const
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
