#include "ResourceVideo.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/VideoCodecInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ConfigInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

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
    bool ResourceVideo::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        m_filePath = metadata->get_File_Path();

        metadata->get_File_Codec( &m_codecType );
        metadata->get_File_Converter( &m_converterType );

        metadata->get_File_Alpha( &m_alpha );
        metadata->get_File_NoSeek( &m_noSeek );
        metadata->get_File_FrameRate( &m_frameRate );
        metadata->get_File_Duration( &m_duration );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_convert()
    {
        bool result = this->convertDefault_( m_converterType, m_filePath, m_filePath, m_codecType );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::_isValid() const
    {
        VideoDecoderInterfacePtr decoder = this->createVideoDecoder();

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "ResourceVideo::_isValid invalid create decoder '%s' path '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        bool valid = this->checkValidVideoDecoder_( decoder );

        this->destroyVideoDecoder( decoder );

        return valid;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideo::checkValidVideoDecoder_( const VideoDecoderInterfacePtr & _decoder ) const
    {
        if( _decoder == nullptr )
        {
            LOGGER_ERROR( "ResourceVideo::isValid: group '%s' name '%s' can't create decoder '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        const VideoCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

        const uint32_t MENGINE_VIDEO_SIZE_DIV = 8;

        if( dataInfo->frameWidth % MENGINE_VIDEO_SIZE_DIV != 0 ||
            dataInfo->frameHeight % MENGINE_VIDEO_SIZE_DIV != 0 )
        {
            LOGGER_ERROR( "ResourceVideo::checkValidVideoDecoder_ invalid width or heigth '%d:%d' need '%d:%d' maybe div %d"
                , dataInfo->frameWidth
                , dataInfo->frameHeight
                , (dataInfo->frameWidth / MENGINE_VIDEO_SIZE_DIV + 1) * MENGINE_VIDEO_SIZE_DIV
                , (dataInfo->frameHeight / MENGINE_VIDEO_SIZE_DIV + 1) * MENGINE_VIDEO_SIZE_DIV
                , MENGINE_VIDEO_SIZE_DIV
            );

            return false;
        }

        uint32_t limitVideoWidth = CONFIG_VALUE( "Limit", "VideoWidth", 2048U );
        uint32_t limitVideoHeight = CONFIG_VALUE( "Limit", "VideoHeight", 2048U );

        if( dataInfo->width > limitVideoWidth || dataInfo->height > limitVideoHeight )
        {
            LOGGER_ERROR( "ResourceVideo.isValid: group '%s' name '%s' path '%s' invalid size %d:%d limit %d:%d"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
                , dataInfo->width
                , dataInfo->height
                , limitVideoWidth
                , limitVideoHeight
            );

            return false;
        }

        float Limit_VideoFrameRate = CONFIG_VALUE( "Limit", "VideoFrameRate", 30.f );

        if( dataInfo->fps > Limit_VideoFrameRate && Limit_VideoFrameRate != 0.0 )
        {
            LOGGER_ERROR( "ResourceVideo.isValid: group '%s' name '%s' path '%s' invalid Frame rate %f more that %f"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
                , dataInfo->fps
                , Limit_VideoFrameRate
            );

            return false;
        }

        return true;
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
    VideoDecoderInterfacePtr ResourceVideo::createVideoDecoder() const
    {
        VideoDecoderInterfacePtr cacheVideoDecoder = m_videoDecoderCacher.findCache();

        if( cacheVideoDecoder != nullptr )
        {
            cacheVideoDecoder->rewind();

            return cacheVideoDecoder;
        }

        const FileGroupInterfacePtr & category = this->getCategory();

        InputStreamInterfacePtr videoStream = FILE_SERVICE()
            ->openInputFile( category, m_filePath, true );

        if( videoStream == nullptr )
        {
            LOGGER_ERROR( "ResourceVideo::createVideDecoder group '%s' name '%s' can't open video file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
            );

            return nullptr;
        }

        VideoDecoderInterfacePtr videoDecoder = CODEC_SERVICE()
            ->createDecoderT<VideoDecoderInterfacePtr>( m_codecType );

        if( videoDecoder == nullptr )
        {
            LOGGER_ERROR( "ResourceVideo::createVideDecoder group '%s' name '%s' can't create video decoder for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
            );

            return nullptr;
        }

        VideoCodecOptions videoCodecOptions;

        if( m_alpha == true )
        {
            videoCodecOptions.pixelFormat = PF_A8R8G8B8;
        }
        else
        {
            if( RENDER_SYSTEM()->supportTextureFormat( PF_R8G8B8 ) == true )
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
            LOGGER_ERROR( "ResourceVideo::createVideDecoder group '%s' name '%s' can't setup options for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
            );

            return nullptr;
        }

        if( videoDecoder->prepareData( videoStream ) == false )
        {
            LOGGER_ERROR( "ResourceVideo::createVideDecoder group '%s' name '%s' can't initialize video decoder for file '%s'"
                , this->getGroupName().c_str()
                , this->getName().c_str()
                , m_filePath.c_str()
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
    bool ResourceVideo::isNoSkeep() const
    {
        return m_noSeek;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setFilePath( const FilePath & _path )
    {
        m_filePath = _path;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceVideo::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceVideo::setCodecType( const ConstString & _type )
    {
        m_codecType = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceVideo::getCodecType() const
    {
        return m_codecType;
    }
    //////////////////////////////////////////////////////////////////////////
}
