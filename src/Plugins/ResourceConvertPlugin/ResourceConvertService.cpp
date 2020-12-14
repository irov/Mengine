#include "ResourceConvertService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceConvertService, Mengine::ResourceConvertService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertService::ResourceConvertService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertService::~ResourceConvertService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, &ResourceConvertService::convertResource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Converter" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertService::convertResource_( const ResourcePtr & _resource )
    {
        ContentInterface * content = _resource->getContent();

        if( content == nullptr )
        {
            return;
        }

        const ConstString & converterType = content->getConverterType();

        if( converterType.empty() == true )
        {
            return;
        }

        LOGGER_INFO( "convert", "convert resource '%s' type '%s' group '%s' file locale '%s' converter '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getLocale().c_str()
            , content->getConverterType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        FilePath newFilePath;
        if( CONVERTER_SERVICE()
            ->convert( converterType, fileGroup, filePath, &newFilePath, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't convert '%s':'%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
                , content->getConverterType().c_str()
            );

            throw ExceptionNotificationFailed();
        }

        content->setFilePath( newFilePath );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( newFilePath );

        content->setCodecType( codecType );

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), codecType );

        content->setDataflow( dataflow );
    }
}