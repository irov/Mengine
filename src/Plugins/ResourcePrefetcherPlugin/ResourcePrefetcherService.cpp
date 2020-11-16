#include "ResourcePrefetcherService.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

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
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ) );
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::prefetchResource( const ResourcePtr & _resource, const PrefetcherObserverInterfacePtr & _observer )
    {
        LOGGER_INFO( "prefetch", "prefetch resource '%s' type '%s' group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        ResourcePrefetcherInterfacePtr prefetcher = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), resourceType );

        if( prefetcher == nullptr )
        {
            return false;
        }

        bool successful = prefetcher->prefetch( _resource, _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherService::unfetchResource( const ResourcePtr & _resource )
    {
        LOGGER_INFO( "prefetch", "unfetch resource '%s' type '%s' group '%s' locale '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getLocale().c_str()
        );

        const ConstString & resourceType = _resource->getType();

        ResourcePrefetcherInterfacePtr prefetcher = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), resourceType );

        if( prefetcher == nullptr )
        {
            return false;
        }

        bool successful = prefetcher->unfetch( _resource );

        return successful;
    }
}