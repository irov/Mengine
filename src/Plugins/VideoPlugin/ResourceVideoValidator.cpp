#include "ResourceVideoValidator.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceVideoValidator::ResourceVideoValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceVideoValidator::~ResourceVideoValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_checkValidVideoDecoder( const ResourceVideoPtr & _resource, const VideoDecoderInterfacePtr & _decoder )
    {
        const VideoCodecDataInfo * dataInfo = _decoder->getCodecDataInfo();

        bool successful = true;

        const uint32_t MENGINE_VIDEO_SIZE_DIV = 8;

        if( dataInfo->frameWidth % MENGINE_VIDEO_SIZE_DIV != 0 ||
            dataInfo->frameHeight % MENGINE_VIDEO_SIZE_DIV != 0 )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' invalid width or heigth '%d:%d' need '%d:%d' maybe div %d"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , dataInfo->frameWidth
                , dataInfo->frameHeight
                , (dataInfo->frameWidth / MENGINE_VIDEO_SIZE_DIV + 1) * MENGINE_VIDEO_SIZE_DIV
                , (dataInfo->frameHeight / MENGINE_VIDEO_SIZE_DIV + 1) * MENGINE_VIDEO_SIZE_DIV
                , MENGINE_VIDEO_SIZE_DIV
            );

            successful = false;
        }

        uint32_t Limit_VideoWidth = CONFIG_VALUE( "Limit", "VideoWidth", 2048U );
        uint32_t Limit_VideoHeight = CONFIG_VALUE( "Limit", "VideoHeight", 2048U );

        if( dataInfo->width > Limit_VideoWidth || dataInfo->height > Limit_VideoHeight )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' override size %d:%d limit %d:%d"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , dataInfo->width
                , dataInfo->height
                , Limit_VideoWidth
                , Limit_VideoHeight
            );

            successful = false;
        }

        float Limit_VideoFrameRate = CONFIG_VALUE( "Limit", "VideoFrameRate", 30.f );

        if( dataInfo->fps > Limit_VideoFrameRate && Limit_VideoFrameRate != 0.0 )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' override frame rate %f more that %f"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , dataInfo->fps
                , Limit_VideoFrameRate
            );

            successful = false;
        }

        float Limit_VideoContentFillrateCoeff = CONFIG_VALUE( "Limit", "VideoContentFillrateCoeff", 1.f );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float videoFillrate = (float)(dataInfo->width * dataInfo->height);
        
        float resolutionWidth = resolution.getWidthF();
        float resolutionHeight = resolution.getHeightF();

        float resolutionFillrate = resolutionWidth * resolutionHeight;

        if( videoFillrate > resolutionFillrate * Limit_VideoContentFillrateCoeff )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' override fillrate %f [coeff %f]"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , videoFillrate / resolutionFillrate
                , Limit_VideoContentFillrateCoeff
            );

            successful = false;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceVideoValidator::_validate( const ResourceVideoPtr & _resource )
    {
        VideoDecoderInterfacePtr decoder = _resource->createVideoDecoder( MENGINE_DOCUMENT_FACTORABLE );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't create decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        bool valid = s_checkValidVideoDecoder( _resource, decoder );

        _resource->destroyVideoDecoder( decoder );

        return valid;
    }
    //////////////////////////////////////////////////////////////////////////
}