#include "ResourceVideoValidator.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
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

        uint32_t Limit_VideoSizeDiv = CONFIG_VALUE( "Limit", "VideoSizeDiv", 8U );

        if( dataInfo->frameWidth % Limit_VideoSizeDiv != 0 ||
            dataInfo->frameHeight % Limit_VideoSizeDiv != 0 )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' invalid width or heigth [%u:%u] need [%u:%u] maybe div [%u]"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , dataInfo->frameWidth
                , dataInfo->frameHeight
                , (dataInfo->frameWidth / Limit_VideoSizeDiv + 1) * Limit_VideoSizeDiv
                , (dataInfo->frameHeight / Limit_VideoSizeDiv + 1) * Limit_VideoSizeDiv
                , Limit_VideoSizeDiv
            );

            successful = false;
        }

        uint32_t Limit_VideoWidth = CONFIG_VALUE( "Limit", "VideoWidth", 2048U );
        uint32_t Limit_VideoHeight = CONFIG_VALUE( "Limit", "VideoHeight", 2048U );

        if( dataInfo->width > Limit_VideoWidth || dataInfo->height > Limit_VideoHeight )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' override size [%u:%u] limit [%u:%u]"
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

        float frameRate = _resource->getFrameRate();

        if( frameRate > Limit_VideoFrameRate && Limit_VideoFrameRate != 0.f )
        {
            LOGGER_ERROR( "resource '%s' group '%s' path '%s' override frame rate %f more that %f"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
                , frameRate
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
    bool ResourceVideoValidator::_validate( const ResourceVideoPtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            bool validNoExist = content->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' not exist file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getFileGroupFullPath( content->getFileGroup(), content->getFilePath() )
            );

            return false;
        }

        InputStreamInterfacePtr videoStream = Helper::openInputStreamFile( fileGroup, filePath, true, false, MENGINE_DOCUMENT_FACTORABLE );

        if( videoStream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't open video file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        VideoDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( decoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't create video decoder for file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        if( decoder->prepareData( videoStream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' can't initialize video decoder for file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        bool valid = s_checkValidVideoDecoder( _resource, decoder );

        return valid;
    }
    //////////////////////////////////////////////////////////////////////////
}