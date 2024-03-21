#include "ResourceService.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Error.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/Algorithm.h"
#include "Config/Utility.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceService, Mengine::ResourceService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    /////////////////////////////////////////////////////////////////////
    ResourceService::ResourceService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceService::~ResourceService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::_initializeService()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        m_factoryResourceBank = Helper::makeFactoryPool<ResourceBank, 8>( MENGINE_DOCUMENT_FACTORABLE );

        ResourceBankPtr globalBank = m_factoryResourceBank->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( globalBank );

        uint32_t Engine_ResourceHashTableSize = CONFIG_VALUE( "Engine", "ResourceHashTableSize", 1024 * 32 );

        if( globalBank->initialize( m_mutex, Engine_ResourceHashTableSize ) == false )
        {
            return false;
        }

        m_globalBank = globalBank;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_FINALIZE, &ResourceService::notifyEngineFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_FINALIZE );

        m_mutex = nullptr;

        m_resourcesCache.clear();

        m_globalBank = nullptr;
        
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryResourceBank );

        m_factoryResourceBank = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::_stopService()
    {
        m_globalBank->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::notifyEngineFinalize_()
    {
        m_globalBank->finalizeKeepResource();
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceBankInterfacePtr ResourceService::createResourceBank( uint32_t _reserved, const DocumentInterfacePtr & _doc )
    {
        ResourceBankPtr bank = m_factoryResourceBank->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( bank );

        if( bank->initialize( m_mutex, _reserved ) == false )
        {
            return nullptr;
        }

        return bank;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePointer ResourceService::createResource( const ResourceCook & _cook, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( !(_cook.name.empty() == true && _cook.groupCache == true), "resource '%s' can't be cacheable group '%s'"
            , _cook.name.c_str()
            , _cook.groupName.c_str()
        );

        Resource * prev_resource = nullptr;
        ResourcePtr resource = m_globalBank->createResource( _cook, &prev_resource, _doc );

        //ToDo

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid generate resource group '%s' name '%s' type '%s' doc '%s'"
            //, _locales.c_str()
            , _cook.groupName.c_str()
            , _cook.name.c_str()
            , _cook.type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( _cook.name.empty() == false )
        {
            resource->setGroupCache( _cook.groupCache );

            if( _cook.groupCache == true )
            {
                MapResourceCache::iterator it_cache_found = m_resourcesCache.find( _cook.groupName );

                if( it_cache_found == m_resourcesCache.end() )
                {
                    it_cache_found = m_resourcesCache.insert( it_cache_found, Utility::make_pair( _cook.groupName, VectorResources() ) );
                }

                VectorResources & cahce_resources = it_cache_found->second;

                cahce_resources.emplace_back( resource );

                if( prev_resource != nullptr )
                {
                    const ConstString & insert_group = prev_resource->getGroupName();

                    MapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( insert_group );

                    MENGINE_ASSERTION_FATAL( it_remove_cache_found != m_resourcesCache.end(), "resource '%s' group '%s' not found"
                        , prev_resource->getName().c_str()
                        , insert_group.c_str()
                    );

                    VectorResources & cache_resources = it_remove_cache_found->second;

                    VectorResources::iterator it_remove_found = Algorithm::remove( cache_resources.begin(), cache_resources.end(), prev_resource );
                    cache_resources.erase( it_remove_found, cache_resources.end() );
                }
            }
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::removeResource( const ResourcePtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resource );

        if( _resource->isGroupCache() == true )
        {
            const ConstString & group = _resource->getGroupName();

            MapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( group );

            MENGINE_ASSERTION_FATAL( it_remove_cache_found != m_resourcesCache.end(), "resource '%s' group '%s' not found"
                , _resource->getName().c_str()
                , group.c_str()
            );

            VectorResources & cache_resources = it_remove_cache_found->second;

            VectorResources::iterator it_found = Algorithm::remove( cache_resources.begin(), cache_resources.end(), _resource );
            cache_resources.erase( it_found, cache_resources.end() );
        }

        m_globalBank->removeResource( _resource );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::hasResource( const ConstString & _groupName, const ConstString & _name, bool _onlyGroup, ResourcePtr * const _resource ) const
    {
        bool exist = m_globalBank->hasResource( _groupName, _name, _onlyGroup, _resource );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceService::getResourceReference( const ConstString & _groupName, const ConstString & _name ) const
    {
        const ResourcePtr & resource = m_globalBank->getResource( _groupName, _name );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::foreachResources( const LambdaResource & _lambda ) const
    {
        const ResourceBank::HashtableResources & resources = m_globalBank->getResources();

        for( const ResourceBank::HashtableResources::value_type & value : resources )
        {
            const ResourcePtr & resource = value.element;

            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::foreachGroupResources( const ConstString & _groupName, const LambdaResource & _lambda ) const
    {
        MapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( _groupName );

        if( it_cache_found == m_resourcesCache.end() )
        {
            return;
        }

        const VectorResources & resources = it_cache_found->second;

        for( const ResourcePtr & resource : resources )
        {
            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::foreachTagsResources( const Tags & _tags, const LambdaResource & _lambda ) const
    {
        const ResourceBank::HashtableResources & resources = m_globalBank->getResources();

        for( const ResourceBank::HashtableResources::value_type & value : resources )
        {
            const ResourcePtr & resource = value.element;

            const Tags & resourceTags = resource->getTags();

            if( resourceTags.hasTags( _tags ) == false )
            {
                continue;
            }

            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::visitResources( const VisitorPtr & _visitor ) const
    {
        const ResourceBank::HashtableResources & resources = m_globalBank->getResources();

        for( const ResourceBank::HashtableResources::value_type & value : resources )
        {
            const ResourcePtr & resource = value.element;

            resource->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::visitGroupResources( const ConstString & _groupName, const VisitorPtr & _visitor ) const
    {
        MapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( _groupName );

        if( it_cache_found == m_resourcesCache.end() )
        {
            return;
        }

        const VectorResources & resources = it_cache_found->second;

        for( const ResourcePtr & resource : resources )
        {
            resource->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
