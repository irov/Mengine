#include "ResourceValidateService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceValidateService, Mengine::ResourceValidateService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceValidateService::ResourceValidateService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceValidateService::~ResourceValidateService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_ENGINE_ENABLE_PACKAGES, this, &ResourceValidateService::visitableResources_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidateService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_ENABLE_PACKAGES );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Validator" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateService::validResource( const ResourcePtr & _resource ) const
    {
        bool valid = this->visitableResource_( _resource );

        return valid;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidateService::visitableResource_( const ResourcePtr & _resource ) const
    {
        const ConstString & resourceType = _resource->getType();

        const ResourceValidatorInterfacePtr & validator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Validator" ), resourceType );

        if( validator == nullptr )
        {
            return true;
        }

        LOGGER_INFO( "validate resource '%s' type '%s' group '%s' file group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getFileGroup()->getName().c_str()
            , _resource->getLocale().c_str()
        );

        bool successful = validator->validate( _resource );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidateService::visitableResources_() const
    {
        bool developmentMode = HAS_OPTION( "dev" );
        bool noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( developmentMode == true && noresourceCheck == false )
        {
            bool successful = true;

            RESOURCE_SERVICE()
                ->foreachResources( [this, &successful]( const ResourcePtr & _resource )
            {
                //#ifndef MENGINE_MASTER_RELEASE
                //                if( _resource->convert() == false )
                //                {
                //                    LOGGER_ERROR( "ResourceValidateService::visitableResources_ %s type [%s] invalid convert"
                //                        , _resource->getName().c_str()
                //                        , _resource->getType().c_str()
                //                    );
                //
                //                    successful = false;
                //
                //                    return;
                //                }
                //#endif

                if( this->visitableResource_( _resource ) == false )
                {
                    successful = false;

                    return;
                }
            } );

            if( successful == false )
            {
                throw ExceptionNotificationFailed();
            }
        }
    }
}