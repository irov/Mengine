#include "ResourceImageDataValidator.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
    bool ResourceImageDataValidator::_validate( const ResourceImageDataPtr & _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        bool exist = fileGroup->existFile( filePath, true );

        if( exist == false )
        {
            bool validNoExist = _resource->isValidNoExist();

            if( validNoExist == true )
            {
                return true;
            }

            LOGGER_ERROR( "resource '%s' group '%s' not exist file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' invalid open file '%s:%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        const ConstString & codecType = _resource->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , _resource->getCodecType().c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' decoder initialize failed '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
                , _resource->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
}