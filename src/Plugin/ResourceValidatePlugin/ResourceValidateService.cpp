#include "ResourceValidateService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/OptionsInterface.h"

#include "Kernel/Assertion.h"
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
        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_ENGINE_ENABLE_PACKAGES, this, &ResourceValidateService::visitableResources_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidateService::_finalizeService()
    {
        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_ENGINE_ENABLE_PACKAGES, this );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidateService::addResourceValidateVisitor( const VisitorPtr & _visitor )
    {
        m_visitors.emplace_back( _visitor );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidateService::removeResourceValidateVisitor( const VisitorPtr & _visitor )
    {
        VectorResourceValidateVisitor::iterator it_erase = std::find( m_visitors.begin(), m_visitors.end(), _visitor );

        MENGINE_ASSERTION( it_erase != m_visitors.end() );

        m_visitors.erase( it_erase );
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
        for( const VisitorPtr & visitor : m_visitors )
        {
            if( _resource->visitIf( visitor ) == false )
            {
                return false;
            }
        }

        return true;
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
                if( this->visitableResource_( _resource ) == false )
                {
                    successful = false;
                }
            } );

            if( successful == false )
            {
                LOGGER_ERROR( "Resources validation is invalid!!!!!!!!!!!!!"
                );

                throw ExceptionNotificationFailed();
            }
        }
    }
}