#include "ResourceManager.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/LoaderInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ConfigInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Loadable.h"
#include "Kernel/Resource.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceService, Mengine::ResourceManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceManager::ResourceManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceManager::~ResourceManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceManager::_initializeService()
    {
        uint32_t ResourceHashTableSize = CONFIG_VALUE( "Engine", "ResourceHashTableSize", 1024 * 32 );

        m_resources.reserve( ResourceHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::_finalizeService()
    {
        m_resourcesCache.clear();

#ifndef MENGINE_MASTER_RELEASE
        for( const ResourcePtr & resource : m_resources )
        {
            uint32_t refcount = resource->countReference();

            if( refcount != 0 )
            {
                LOGGER_WARNING( "ResourceManager::~ResourceManager resource '%s' type '%s' group '%s' refcount %d"
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
    bool ResourceManager::loadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pak, const FilePath & _path, bool _ignored )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _pak, _path, &datablock, exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "ResourceManager::loadResource: resource '%s:%s' not found"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "ResourceManager::loadResource: Invalid parse resource '%s:%s'"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include::const_iterator
            it = includes_include.begin(),
            it_end = includes_include.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include = *it;

            const FilePath & path = meta_include.get_Path();

            if( this->loadResources( _locale, _pak, path, _ignored ) == false )
            {
                LOGGER_ERROR( "ResourceManager::loadResource load %s:%s resource invalid load include %s"
                    , _pak->getName().c_str()
                    , _path.c_str()
                    , path.c_str()
                );

                return false;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource::const_iterator
            it = includes_resource.begin(),
            it_end = includes_resource.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource = *it;

            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            bool unique = true;
            meta_resource->get_Unique( &unique );

            ResourcePtr has_resource = nullptr;
            if( this->hasResource( name, &has_resource ) == true )
            {
                if( unique == false )
                {
                    continue;
                }

                const FileGroupInterfacePtr & resource_category = has_resource->getFileGroup();

                LOGGER_ERROR( "ResourceManager::loadResource: path %s already exist resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
                    , _path.c_str()
                    , name.c_str()
                    , groupName.c_str()
                    , _pak->getName().c_str()
                    , resource_category->getName().c_str()
                    , has_resource->getFileGroup()->getName().c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            ResourcePtr resource =
                this->createResource( _locale, _pak, groupName, name, type );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "ResourceManager::loadResource: '%s' invalid create resource '%s:%s' name %s type %s"
                    , _path.c_str()
                    , _pak->getName().c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                );

                return false;
            }

            if( resource->loader( meta_resource ) == false )
            {
                LOGGER_ERROR( "ResourceManager::loadResource '%s' category '%s' group '%s' name '%s' type '%s' invalid load"
                    , _path.c_str()
                    , _pak->getName().c_str()
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
                resource->incrementReference();
            }

#ifndef MENGINE_MASTER_RELEASE
            if( _ignored == false && resource->convert() == false )
            {
                LOGGER_ERROR( "ResourceManager::loadResource %s type [%s] invalid convert"
                    , name.c_str()
                    , type.c_str()
                );

                continue;
            }
#endif
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceManager::unloadResources( const ConstString & _locale, const FileGroupInterfacePtr & _pak, const FilePath & _path )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _pak, _path, &datablock, exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "ResourceManager::unloadResource: resource '%s:%s' not found"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "ResourceManager::unloadResource: Invalid parse resource '%s:%s'"
                    , _pak->getName().c_str()
                    , _path.c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include::const_iterator
            it = includes_include.begin(),
            it_end = includes_include.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include = *it;

            const FilePath & path = meta_include.get_Path();

            if( this->unloadResources( _locale, _pak, path ) == false )
            {
                LOGGER_ERROR( "ResourceManager::unloadResource load %s:%s resource invalid load include %s"
                    , _pak->getName().c_str()
                    , _path.c_str()
                    , path.c_str()
                );

                return false;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource::const_iterator
            it = includes_resource.begin(),
            it_end = includes_resource.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource = *it;

            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

#ifndef MENGINE_MASTER_RELEASE
            ResourcePtr has_resource = nullptr;
            if( this->hasResource( name, &has_resource ) == false )
            {
                const FileGroupInterfacePtr & resource_category = has_resource->getFileGroup();

                LOGGER_ERROR( "ResourceManager::unloadResource: path %s not found resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
                    , _path.c_str()
                    , name.c_str()
                    , groupName.c_str()
                    , _pak->getName().c_str()
                    , resource_category->getName().c_str()
                    , has_resource->getFileGroup()->getName().c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }
#endif

            if( this->removeResource( has_resource ) == false )
            {
                LOGGER_ERROR( "ResourceManager::unloadResource: '%s' invalid remove resource '%s:%s' name %s type %s"
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
    PointerResourceReference ResourceManager::generateResource( const ConstString& _type ) const
    {
        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), _type );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "ResourceManager::generateResource not registered resource type '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        LOGGER_INFO( "ResourceManager::generateResource type '%s'"
            , _type.c_str()
        );

        if( resource->initialize() == false )
        {
            return nullptr;
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    PointerResourceReference ResourceManager::createResource( const ConstString & _locale, const FileGroupInterfacePtr& _category, const ConstString& _groupName, const ConstString& _name, const ConstString& _type )
    {
        ResourcePtr resource = this->generateResource( _type );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "ResourceManager createResource: invalid generate resource locale '%s' category '%s' group '%s' name '%s' type '%s'"
                , _locale.c_str()
                , _category->getName().c_str()
                , _groupName.c_str()
                , _name.c_str()
                , _type.c_str()
            );

            return nullptr;
        }

        resource->setLocale( _locale );
        resource->setFileGroup( _category );
        resource->setGroupName( _groupName );
        resource->setName( _name );

        ResourceEntry entry;

        entry.resource = resource;

        const ResourcePtr & prev_resource = m_resources.change( _name, resource );

        ResourceCacheKey cache_key = std::make_pair( _category->getName(), _groupName );

        MapResourceCache::iterator it_cache_found = m_resourcesCache.find( cache_key );

        if( it_cache_found == m_resourcesCache.end() )
        {
            it_cache_found = m_resourcesCache.insert( it_cache_found, std::make_pair( cache_key, VectorResources() ) );
        }

        VectorResources & cahce_resources = it_cache_found->second;

        cahce_resources.emplace_back( resource );

        if( prev_resource != nullptr )
        {
            const FileGroupInterfacePtr & insert_category = prev_resource->getFileGroup();
            const ConstString & insert_group = prev_resource->getGroupName();

            ResourceCacheKey remove_cache_key = std::make_pair( insert_category->getName(), insert_group );

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
    bool ResourceManager::removeResource( const ResourcePtr & _resource )
    {
        if( _resource == nullptr )
        {
            return false;
        }

        const ConstString & name = _resource->getName();

        if( m_resources.remove( name ) == false )
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
    bool ResourceManager::hasResource( const ConstString& _name, ResourcePtr * _resource ) const
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
    bool ResourceManager::hasResourceWithType( const ConstString& _name, const ConstString& _type ) const
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
    const ResourcePtr & ResourceManager::getResource( const ConstString& _name ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "ResourceManager::getResource: resource '%s' does not exist"
                , _name.c_str()
            );

            return ResourcePtr::none();
        }

        if( resource->incrementReference() == false )
        {
            LOGGER_ERROR( "ResourceManager::getResource: resource '%s' '%s' is not compile!"
                , _name.c_str()
                , resource->getType().c_str()
            );

            return ResourcePtr::none();
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceManager::getResourceReference( const ConstString & _name ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        if( resource == nullptr )
        {
            LOGGER_WARNING( "ResourceManager::getResourceReference: resource '%s' does not exist"
                , _name.c_str()
            );

            return ResourcePtr::none();
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceManager::getResourceType( const ConstString & _name ) const
    {
        const ResourcePtr & resource = m_resources.find( _name );

        if( resource == nullptr )
        {
            LOGGER_WARNING( "ResourceManager::getResourceType: resource '%s' does not exist"
                , _name.c_str()
            );

            return ConstString::none();
        }

        const ConstString & type = resource->getType();

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::foreachResources( const LambdaResource & _lambda ) const
    {
        for( const ResourcePtr & resource : m_resources )
        {
            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::visitResources( const VisitorPtr & _visitor ) const
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->visit( _visitor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::visitGroupResources( const FileGroupInterfacePtr & _category, const ConstString & _group, const VisitorPtr & _visitor ) const
    {
        ResourceCacheKey cache_key = std::make_pair( _category->getName(), _group );

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
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class FResourcesForeachDump
        {
        public:
            FResourcesForeachDump()
            {
            }

        private:
            void operator = ( const FResourcesForeachDump & )
            {
            }

        public:
            void operator () ( ResourceEntry * _entry )
            {
                const ResourcePtr & resource = _entry->resource;

                uint32_t count = resource->countReference();

                if( count == 0 )
                {
                    return;
                }

                if( resource->isCompile() == false )
                {
                    return;
                }

                const ConstString & name = _entry->resource->getName();

                LOGGER_ERROR( "Resource %s\n: count - %u"
                    , name.c_str()
                    , count
                );
            }
        };
    }
}
