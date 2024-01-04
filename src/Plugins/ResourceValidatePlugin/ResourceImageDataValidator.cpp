#include "ResourceImageDataValidator.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDataValidator::ResourceImageDataValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageDataValidator::~ResourceImageDataValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageDataValidator::_validate( const ResourceImageDataPtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        if( content->exist( true ) == false )
        {
            bool validNoExist = content->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' not exist file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() )
            );

            return false;
        }

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid open file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() )
            );

            return false;
        }

        if( stream->size() == 0 )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s' codec '%s' empty"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() )
                , _resource->getContent()->getCodecType().c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( imageDecoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() )
                , _resource->getContent()->getCodecType().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s' decoder initialize failed '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Helper::getContentFullPath( _resource->getContent() )
                , _resource->getContent()->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}