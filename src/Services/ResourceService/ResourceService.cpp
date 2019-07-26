#include "ResourceService.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include <algorithm>

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
        m_mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        uint32_t ResourceHashTableSize = CONFIG_VALUE( "Engine", "ResourceHashTableSize", 1024 * 32 );

        m_resources.reserve( ResourceHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::_finalizeService()
    {
        m_mutex = nullptr;

        m_resourcesCache.clear();

#ifndef MENGINE_MASTER_RELEASE
        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtr & resource = value.element;

            uint32_t refcount = resource->getCompileReferenceCount();

            if( refcount != 0 )
            {
                LOGGER_WARNING( "resource '%s' type '%s' group '%s' refcount %d"
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                    , resource->getGroupName().c_str()
                    , refcount
                );
            }
        }
#endif

        m_resources.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Tags & _tags, bool _ignored )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _filePath, &datablock, Metacode::Meta_Data::getVersion(), exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "resource '%s:%s' not found"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resource '%s:%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

            if( this->loadResources( _locale, _fileGroup, path, _tags, _ignored ) == false )
            {
                LOGGER_ERROR( "load '%s:%s' resource invalid load include '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , path.c_str()
                );

                return false;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            bool unique = true;
            meta_resource->get_Unique( &unique );

            ResourcePtr has_resource;
            if( this->hasResource( name, &has_resource ) == true )
            {
                if( unique == false )
                {
                    continue;
                }

                const FileGroupInterfacePtr & resource_fileGroup = has_resource->getFileGroup();

                LOGGER_ERROR( "path '%s' already exist resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
                    , _filePath.c_str()
                    , name.c_str()
                    , groupName.c_str()
                    , _fileGroup->getName().c_str()
                    , resource_fileGroup->getName().c_str()
                    , has_resource->getFileGroup()->getName().c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            ResourcePtr resource = this->createResource( _locale, _fileGroup, groupName, name, type, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, false, "file '%s' invalid create resource '%s:%s' name '%s' type '%s'"
                , _filePath.c_str()
                , _fileGroup->getName().c_str()
                , groupName.c_str()
                , name.c_str()
                , type.c_str()
            );

            resource->setTags( _tags );

            LoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Loader" ), type );

            MENGINE_ASSERTION_MEMORY_PANIC( loader, false, "file '%s' resource '%s:%s' invalid create loader '%s'"
                , _filePath.c_str()
                , _fileGroup->getName().c_str()
                , groupName.c_str()
                , type.c_str()
            );

            if( loader->load( resource, meta_resource ) == false )
            {
                LOGGER_ERROR( "file '%s' category '%s' group '%s' name '%s' type '%s' invalid load"
                    , _filePath.c_str()
                    , _fileGroup->getName().c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                );

                continue;
            }

            bool precompile = false;
            meta_resource->get_Precompile( &precompile );

            if( precompile == true )
            {
                resource->compile();
            }

#ifndef MENGINE_MASTER_RELEASE
            if( _ignored == false )
            {
                if( NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, resource ) == false )
                {
                    LOGGER_ERROR( "resource '%s' type [%s] invalid convert"
                        , name.c_str()
                        , type.c_str()
                    );

                    return false;
                }
            }
#endif
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pak, const FilePath & _path )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _pak, _path, &datablock, Metacode::Meta_Data::getVersion(), exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "resource '%s:%s' not found"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resource '%s:%s'"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

            if( this->unloadResources( _locale, _pak, path ) == false )
            {
                LOGGER_ERROR( "load '%s:%s' resource invalid load include '%s'"
                    , _pak->getName().c_str()
                    , _path.c_str()
                    , path.c_str()
                );

                return false;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            ResourcePtr has_resource = nullptr;
            if( this->hasResource( name, &has_resource ) == false )
            {
                const FileGroupInterfacePtr & resource_fileGroup = has_resource->getFileGroup();

                LOGGER_ERROR( "path '%s' not found resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
                    , _path.c_str()
                    , name.c_str()
                    , groupName.c_str()
                    , _pak->getName().c_str()
                    , resource_fileGroup->getName().c_str()
                    , has_resource->getFileGroup()->getName().c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            if( this->removeResource( has_resource ) == false )
            {
                LOGGER_ERROR( "path '%s' invalid remove resource '%s:%s' name '%s' type '%s'"
                    , _path.c_str()
                    , _pak->getName().c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PointerResourceReference ResourceService::generateResource( const ConstString & _type, const Char * _doc ) const
    {
        LOGGER_INFO( "generate resource '%s'"
            , _type.c_str()
        );

        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr, "not registered resource type '%s'"
            , _type.c_str()
        );           

        if( resource->initialize() == false )
        {
            return nullptr;
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    PointerResourceReference ResourceService::createResource( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, const Char * _doc )
    {
        ResourcePtr resource = this->generateResource( _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr, "invalid generate resource locale '%s' category '%s' group '%s' name '%s' type '%s' doc '%s'"
            , _locale.c_str()
            , _fileGroup->getName().c_str()
            , _groupName.c_str()
            , _name.c_str()
            , _type.c_str()
            , _doc
        );

        resource->setLocale( _locale );
        resource->setFileGroup( _fileGroup );
        resource->setGroupName( _groupName );
        resource->setName( _name );

        const ResourcePtr & prev_resource = m_resources.change( _name, resource );

        ResourceCacheKey cache_key = std::make_pair( _fileGroup->getName(), _groupName );

        MapResourceCache::iterator it_cache_found = m_resourcesCache.find( cache_key );

        if( it_cache_found == m_resourcesCache.end() )
        {
            it_cache_found = m_resourcesCache.insert( it_cache_found, std::make_pair( cache_key, VectorResources() ) );
        }

        VectorResources & cahce_resources = it_cache_found->second;

        cahce_resources.emplace_back( resource );

        if( prev_resource != nullptr )
        {
            const FileGroupInterfacePtr & insert_fileGroup = prev_resource->getFileGroup();
            const ConstString & insert_group = prev_resource->getGroupName();

            ResourceCacheKey remove_cache_key = std::make_pair( insert_fileGroup->getName(), insert_group );

            MapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( remove_cache_key );

            VectorResources::iterator it_remove_found = std::remove(
                it_remove_cache_found->second.begin(),
                it_remove_cache_found->second.end(),
                prev_resource );

            it_remove_cache_found->second.erase( it_remove_found );
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::removeResource( const ResourcePtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, false );

        const ConstString & name = _resource->getName();

        if( m_resources.erase( name ) == nullptr )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();
        const ConstString & group = _resource->getGroupName();

        ResourceCacheKey remove_cache_key = std::make_pair( fileGroup->getName(), group );

        MapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( remove_cache_key );

        if( it_remove_cache_found == m_resourcesCache.end() )
        {
            return false;
        }

        VectorResources::iterator it_found = std::remove(
            it_remove_cache_found->second.begin(),
            it_remove_cache_found->second.end(),
            _resource );

        it_remove_cache_found->second.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::hasResource( const ConstString& _name, ResourcePtr * _resource ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        if( resource == nullptr )
        {
            return false;
        }

        if( _resource != nullptr )
        {
            *_resource = resource;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceService::hasResourceWithType( const ConstString& _name, const ConstString& _type ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        if( resource == nullptr )
        {
            return false;
        }

        const ConstString & resourceType = resource->getType();

        if( resourceType != _type )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceService::getResource( const ConstString& _name ) const
    {
        MENGINE_ASSERTION_MAIN_THREAD_GUARD();

        const ResourcePtr & resource = m_resources.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, ResourcePtr::none(), "resource '%s' does not exist"
            , _name.c_str()
        );

        if( resource->compile() == false )
        {
            LOGGER_ERROR( "resource '%s' '%s' is not compile!"
                , _name.c_str()
                , resource->getType().c_str()
            );

            return ResourcePtr::none();
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceService::getResourceReference( const ConstString & _name ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        const ResourcePtr & resource = m_resources.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, ResourcePtr::none(), "resource '%s' does not exist"
            , _name.c_str()
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceService::getResourceType( const ConstString & _name ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, ConstString::none(), "resource '%s' does not exist"
            , _name.c_str()
        );

        const ConstString & type = resource->getType();

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::foreachResources( const LambdaResource & _lambda ) const
    {
        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtr & resource = value.element;

            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::foreachGroupResources( const FileGroupInterfacePtr & _fileGroup, const ConstString & _groupName, const LambdaResource & _lambda ) const
    {
        ResourceCacheKey cache_key = std::make_pair( _fileGroup->getName(), _groupName );

        MapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( cache_key );

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
        for( const HashtableResources::value_type & value : m_resources )
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
        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtr & resource = value.element;

            resource->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceService::visitGroupResources( const FileGroupInterfacePtr & _fileGroup, const ConstString & _groupName, const VisitorPtr & _visitor ) const
    {
        ResourceCacheKey cache_key = std::make_pair( _fileGroup->getName(), _groupName );

        MapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( cache_key );

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
}
