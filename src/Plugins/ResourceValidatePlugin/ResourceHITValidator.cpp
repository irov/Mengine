#include "ResourceHITValidator.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/PickCodecInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

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
        const ContentInterface * content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            return false;
        }

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "resource '%s' group '%s' invalid open file '%s:%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , content->getFileGroup()->getName().c_str()
            , content->getFilePath().c_str()
        );

        const ConstString & codecType = content->getCodecType();

        PickDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<PickDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, false, "resource '%s' group '%s' file '%s:%s' invalid decoder '%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , content->getFileGroup()->getName().c_str()
            , content->getFilePath().c_str()
            , content->getCodecType().c_str()
        );

        if( decoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file '%s:%s' decoder initialize failed '%s'"
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
}