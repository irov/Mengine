#include "ResourceConvertService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Content.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, this, &ResourceConvertService::convertResource_ );

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
        Content * content = _resource->getContent();

        if( content == nullptr )
        {
            return;
        }

        const ConstString & converterType = content->getConverterType();

        if( converterType.empty() == true )
        {
            return;
        }

        LOGGER_INFO( "convert resource '%s' type '%s' group '%s' file group '%s' locale '%s' converter '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getFileGroup()->getName().c_str()
            , _resource->getLocale().c_str()
            , converterType.c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        FilePath newFilePath;
        if( CONVERTER_SERVICE()
            ->convert( converterType, fileGroup, filePath, &newFilePath, MENGINE_DOCUMENT_FUNCTION ) == false )
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

        content->setDataflowType( codecType );
        content->setCodecType( codecType );
    }
}