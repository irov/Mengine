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

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        bool exist = fileGroup->existFile( filePath, true );

        if( exist == false )
        {
            bool validNoExist = content->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' not exist file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid open file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        if( stream->size() == 0 )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' stream '%s:%s' codec '%s' empty"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        const ConstString & codecType = content->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( imageDecoder == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file '%s:%s' decoder initialize failed '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
                , content->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}