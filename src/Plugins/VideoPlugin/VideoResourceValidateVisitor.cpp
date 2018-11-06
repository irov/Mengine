#include "VideoResourceValidateVisitor.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VideoResourceValidateVisitor::VideoResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VideoResourceValidateVisitor::~VideoResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_checkValidVideoDecoder( ResourceVideo * _resource, const VideoDecoderInterfacePtr & _decoder )
    {
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
            LOGGER_ERROR( "ResourceVideo.isValid: '%s' group '%s' path '%s' invalid size %d:%d limit %d:%d"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
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
            LOGGER_ERROR( "ResourceVideo.isValid: '%s' group '%s' path '%s' invalid Frame rate %f more that %f"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
                , dataInfo->fps
                , Limit_VideoFrameRate
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoResourceValidateVisitor::accept( ResourceVideo * _resource )
    {
        VideoDecoderInterfacePtr decoder = _resource->createVideoDecoder();

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "ResourceVideo::isValid: name '%s' group '%s' can't create decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        bool valid = s_checkValidVideoDecoder( _resource, decoder );

        _resource->destroyVideoDecoder( decoder );

        return valid;
    }
}