#include "ResourceConvertService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "ResourceConverterInterface.h"

#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Logger.h"

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
        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, this, &ResourceConvertService::convertResource_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertService::_finalizeService()
    {
        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, this );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Converter" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertService::convertResource_( const ResourcePtr & _resource )
    {
        const ConstString & resourceType = _resource->getType();

        ResourceConverterInterfacePtr converter = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "Converter" ), resourceType );

        if( converter == nullptr )
        {
            return;
        }

        LOGGER_INFO( "convert resource '%s' type '%s' group '%s' file group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getFileGroup()->getName().c_str()
            , _resource->getLocale().c_str()
        );


        if( converter->convert( _resource ) == false )
        {
            throw ExceptionNotificationFailed();
        }
    }
}