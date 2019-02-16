#include "ResourceHITValidator.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PickCodecInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceHITValidator::ResourceHITValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceHITValidator::~ResourceHITValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceHITValidator::_validate( const ResourceHITPtr & _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath ) == false )
        {
            return false;
        }

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

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

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<PickDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
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

        if( decoder->prepareData( stream ) == false )
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