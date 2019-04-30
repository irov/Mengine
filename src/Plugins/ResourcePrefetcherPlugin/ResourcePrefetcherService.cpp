#include "ResourcePrefetcherService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourcePrefetcherService, Mengine::ResourcePrefetcherService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherService::ResourcePrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherService::~ResourcePrefetcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherService::_finalizeService()
    {
        MENGINE_ASSERTION( m_prefetchers.empty() == true, ("resource prefetcher service prefetchers not empty '%d'"
            , m_prefetchers.size()
            ) );

        m_prefetchers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherService::addResourcePrefetcher( const ConstString & _type, const ResourcePrefetcherInterfacePtr & _prefetcher )
    {
        m_prefetchers.insert( _type, _prefetcher );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherService::removeResourcePrefetcher( const ConstString & _type )
    {
        m_prefetchers.remove( _type );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        LOGGER_INFO( "prefetch resource '%s' type '%s' group '%s' file group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getFileGroup()->getName().c_str()
            , _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        const ResourcePrefetcherInterfacePtr & prefetcher = m_prefetchers.find( resourceType );

        if( prefetcher == nullptr )
        {
            return true;
        }

        bool successful = prefetcher->prefetch( _resource, _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::unfetchResource( const ResourcePtr & _resource )
    {
        LOGGER_INFO( "unfetch resource '%s' type '%s' group '%s' file group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getFileGroup()->getName().c_str()
            , _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        const ResourcePrefetcherInterfacePtr & prefetcher = m_prefetchers.find( resourceType );

        if( prefetcher == nullptr )
        {
            return false;
        }

        bool successful = prefetcher->unfetch( _resource );

        return successful;
    }
}