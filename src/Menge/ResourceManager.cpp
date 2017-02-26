#	include "ResourceManager.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LoaderInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#	include "Metacode/Metacode.h"

#	include "Kernel/Loadable.h"
#	include "Kernel/ResourceReference.h"

#   include "Consts.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceService, Menge::ResourceManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
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
	bool ResourceManager::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::_finalize()
	{
		m_resourcesCache.clear();

#   ifndef MENGINE_MASTER_RELEASE
		for( uint32_t i = 0; i != MENGINE_RESOURCE_MANAGER_HASH_SIZE; ++i )
		{
			const TMapResource & resources = m_resources[i];

			for( TMapResource::const_iterator
				it = resources.begin(),
				it_end = resources.end();
			it != it_end;
			++it )
			{
				const ResourceEntry & entry = it->second;

				const ResourceReferencePtr & resource = entry.resource;

				uint32_t refcount = resource->countReference();

				if( refcount != 0 )
				{
					LOGGER_WARNING( m_serviceProvider )("ResourceManager::~ResourceManager resource %s refcount %d"
						, resource->getName().c_str()
						, refcount
						);
				}
			}		
		}
#   endif

		for( uint32_t i = 0; i != MENGINE_RESOURCE_MANAGER_HASH_SIZE; ++i )
		{
			TMapResource & resources = m_resources[i];

			resources.clear();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path, bool _ignored )
	{
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LOADER_SERVICE(m_serviceProvider)->load( _pakName, _path, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: resource '%s:%s' not found"
					, _pakName.c_str()
					, _path.c_str()
					);
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: Invalid parse resource '%s:%s'"
                    , _pakName.c_str()
                    , _path.c_str()
					);
			}

			return false;
		}

        ConstString groupName;
        datablock.swap_Name( groupName );

        const Metacode::Meta_DataBlock::TVectorMeta_Include & includes_include = datablock.get_IncludesInclude();

        for( Metacode::Meta_DataBlock::TVectorMeta_Include::const_iterator
            it = includes_include.begin(),
            it_end = includes_include.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_Include & meta_include = *it;

            const FilePath & path = meta_include.get_Path();

			if( this->loadResources( _locale, _pakName, path, _ignored ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource load %s:%s resource invalid load include %s"
                    , _pakName.c_str()
                    , _path.c_str()
                    , path.c_str()
                    );

                return false;
            }
        }

        const Metacode::Meta_DataBlock::TVectorMeta_Resource & includes_resource = datablock.get_IncludesResource();

        for( Metacode::Meta_DataBlock::TVectorMeta_Resource::const_iterator
            it = includes_resource.begin(),
            it_end = includes_resource.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_Resource * meta_resource = *it;

            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

			bool unique = true;
			meta_resource->get_Unique( unique );

			ResourceReferencePtr has_resource = nullptr;
			if( this->hasResource( name, &has_resource ) == true )
			{
				if( unique == false )
				{
					continue;
				}

				const ConstString & resource_category = has_resource->getCategory();

				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: path %s already exist resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
					, _path.c_str()
					, name.c_str()
					, groupName.c_str()
					, _pakName.c_str()
					, resource_category.c_str()
					, has_resource->getCategory().c_str()
					, has_resource->getGroup().c_str()
					, has_resource->getName().c_str()
					);

				return nullptr;
			}

			ResourceReferencePtr resource =
				this->createResource( _locale, _pakName, groupName, name, type );

            if( resource == nullptr )
            {
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::loadResource: '%s' invalid create resource '%s:%s' name %s type %s"
					, _path.c_str()
                    , _pakName.c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                    );

                return false;
            }

            if( resource->loader( meta_resource ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource '%s' category '%s' group '%s' name '%s' type '%s' invalid load"
					, _path.c_str()
					, _pakName.c_str()
					, groupName.c_str()
					, name.c_str()
					, type.c_str()
                    );

                continue;
            }

			bool precompile = false;
			meta_resource->get_Precompile( precompile );

			if( precompile == true )
			{
				resource->incrementReference();
			}
            
#	ifndef MENGINE_MASTER_RELEASE
			if( _ignored == false && resource->convert() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid convert"
					, name.c_str()
					, type.c_str()
					);

				continue;
			}
#	endif
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::unloadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LOADER_SERVICE( m_serviceProvider )->load( _pakName, _path, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::unloadResource: resource '%s:%s' not found"
					, _pakName.c_str()
					, _path.c_str()
					);
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::unloadResource: Invalid parse resource '%s:%s'"
					, _pakName.c_str()
					, _path.c_str()
					);
			}

			return false;
		}

		ConstString groupName;
		datablock.swap_Name( groupName );

		const Metacode::Meta_DataBlock::TVectorMeta_Include & includes_include = datablock.get_IncludesInclude();

		for( Metacode::Meta_DataBlock::TVectorMeta_Include::const_iterator
			it = includes_include.begin(),
			it_end = includes_include.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Include & meta_include = *it;

			const FilePath & path = meta_include.get_Path();

			if( this->unloadResources( _locale, _pakName, path ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::unloadResource load %s:%s resource invalid load include %s"
					, _pakName.c_str()
					, _path.c_str()
					, path.c_str()
					);

				return false;
			}
		}

		const Metacode::Meta_DataBlock::TVectorMeta_Resource & includes_resource = datablock.get_IncludesResource();

		for( Metacode::Meta_DataBlock::TVectorMeta_Resource::const_iterator
			it = includes_resource.begin(),
			it_end = includes_resource.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Resource * meta_resource = *it;

			const ConstString & name = meta_resource->get_Name();
			const ConstString & type = meta_resource->get_Type();

			ResourceReferencePtr has_resource = nullptr;
			if( this->hasResource( name, &has_resource ) == false )
			{
				const ConstString & resource_category = has_resource->getCategory();

				LOGGER_ERROR( m_serviceProvider )("ResourceManager::unloadResource: path %s not found resource name '%s' in group '%s' category '%s' ('%s')\nhas resource category '%s' group '%s' name '%s'"
					, _path.c_str()
					, name.c_str()
					, groupName.c_str()
					, _pakName.c_str()
					, resource_category.c_str()
					, has_resource->getCategory().c_str()
					, has_resource->getGroup().c_str()
					, has_resource->getName().c_str()
					);

				return nullptr;
			}

			if( this->removeResource( has_resource ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::unloadResource: '%s' invalid remove resource '%s:%s' name %s type %s"
					, _path.c_str()
					, _pakName.c_str()
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
	bool ResourceManager::validateResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) const
    {
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LOADER_SERVICE( m_serviceProvider )->load( _pakName, _path, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources resource '%s:%s' not found"
					, _pakName.c_str()
					, _path.c_str()
					);
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources Invalid parse resource '%s:%s'"
					, _pakName.c_str()
					, _path.c_str()
					);
			}

			return false;
		}

		bool successful = true;

		ConstString groupName;
		datablock.swap_Name( groupName );

		const Metacode::Meta_DataBlock::TVectorMeta_Include & includes_include = datablock.get_IncludesInclude();

		for( Metacode::Meta_DataBlock::TVectorMeta_Include::const_iterator
			it = includes_include.begin(),
			it_end = includes_include.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Include & meta_include = *it;

			const FilePath & path = meta_include.get_Path();

			if( this->validateResources( _locale, _pakName, path ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources load %s:%s resource invalid load include %s"
					, _pakName.c_str()
					, _path.c_str()
					, path.c_str()
					);

				successful = false;

				continue;
			}
		}

		const Metacode::Meta_DataBlock::TVectorMeta_Resource & includes_resource = datablock.get_IncludesResource();

		for( Metacode::Meta_DataBlock::TVectorMeta_Resource::const_iterator
			it = includes_resource.begin(),
			it_end = includes_resource.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Resource * meta_resource = *it;

			const ConstString & name = meta_resource->get_Name();
			const ConstString & type = meta_resource->get_Type();

			bool unique = true;
			meta_resource->get_Unique( unique );

			ResourceReferencePtr resource =
				this->generateResource( type );

			if( resource == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources '%s' invalid create resource '%s:%s' name %s type %s"
					, _path.c_str()
					, _pakName.c_str()
					, groupName.c_str()
					, name.c_str()
					, type.c_str()
					);

				successful = false;

				continue;
			}

			resource->setLocale( _locale );
			resource->setCategory( _pakName );
			resource->setGroup( groupName );
			resource->setName( name );

			if( resource->loader( meta_resource ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources '%s' category '%s' group '%s' name '%s' type '%s' invalid load"
					, _path.c_str()
					, _pakName.c_str()
					, groupName.c_str()
					, name.c_str()
					, type.c_str()
					);

				successful = false;

				continue;
			}

#	ifndef MENGINE_MASTER_RELEASE
			if( resource->convert() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources %s type [%s] invalid convert"
					, name.c_str()
					, type.c_str()
					);

				successful = false;

				continue;
			}
#	endif

			if( resource->isValid() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceManager::validateResources %s type [%s] invalidate"
					, name.c_str()
					, type.c_str()
					);

				successful = false;

				continue;
			}
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReferencePtr ResourceManager::generateResource( const ConstString& _type ) const
	{
		ResourceReferencePtr resource = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototypeT<ResourceReference *>( CONST_STRING( m_serviceProvider, Resource ), _type );

		if( resource == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceManager::generateResource not registered resource type '%s'"
				, _type.c_str()
				);

			return nullptr;
		}

		LOGGER_INFO( m_serviceProvider )("ResourceManager::generateResource type %s"
			, _type.c_str()
			);

		if( resource->initialize() == false )
		{
			return nullptr;
		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReferencePtr ResourceManager::createResource( const ConstString & _locale, const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
	{
		ResourceReferencePtr resource = this->generateResource( _type );

		if( resource == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceManager createResource: invalid generate resource locale '%s' category '%s' group '%s' name '%s' type '%s'"
				, _locale.c_str()
				, _category.c_str()
				, _group.c_str()
				, _name.c_str()
				, _type.c_str()
				);

			return nullptr;
		}

		resource->setLocale( _locale );
		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );

		ResourceEntry entry;
				
		entry.resource = resource;
		entry.isLocked = false;

		ConstString::hash_type hash = _name.hash();
		uint32_t table = (uint32_t)hash % MENGINE_RESOURCE_MANAGER_HASH_SIZE;
		TMapResource & resources = m_resources[table];

		std::pair<TMapResource::iterator, bool> insert_result = resources.insert( std::make_pair( _name, entry ) );

		TResourceCacheKey cache_key = std::make_pair( _category, _group );

		TMapResourceCache::iterator it_cache_found = m_resourcesCache.find( cache_key );

		if( it_cache_found == m_resourcesCache.end() )
		{
			it_cache_found = m_resourcesCache.insert( it_cache_found, std::make_pair( cache_key, TVectorResources() ) );
		}
		
		TVectorResources & cahce_resources = it_cache_found->second;

		cahce_resources.push_back( resource );

		if( insert_result.second == false )
		{
			ResourceEntry & insert_entry = insert_result.first->second;

			const ConstString & insert_category = insert_entry.resource->getCategory();
			const ConstString & insert_group = insert_entry.resource->getGroup();

			TResourceCacheKey remove_cache_key = std::make_pair( insert_category, insert_group );

			TMapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( remove_cache_key );

			TVectorResources::iterator it_remove_found = std::remove(
				it_remove_cache_found->second.begin(),
				it_remove_cache_found->second.end(),
				insert_entry.resource );

			it_remove_cache_found->second.erase( it_remove_found );

			resources[_name] = entry;
		}
        
		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::removeResource( const ResourceReferencePtr & _resource )
	{
		if( _resource == nullptr )
		{
			return false;
		}
		
		const ConstString & name = _resource->getName();

		ConstString::hash_type hash = name.hash();
		TMapResource & resources = m_resources[hash % MENGINE_RESOURCE_MANAGER_HASH_SIZE];
		if( resources.erase( name ) == false )
		{
			return false;
		}

		const ConstString & category = _resource->getCategory();
		const ConstString & group = _resource->getGroup();

		TResourceCacheKey remove_cache_key = std::make_pair( category, group );

		TMapResourceCache::iterator it_remove_cache_found = m_resourcesCache.find( remove_cache_key );
				
		if( it_remove_cache_found == m_resourcesCache.end() )
		{
			return false;
		}

		TVectorResources::iterator it_found = std::remove(
			it_remove_cache_found->second.begin(),
			it_remove_cache_found->second.end(),
			_resource );

		it_remove_cache_found->second.erase( it_found );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			return false;
		}
				
		if( _resource != nullptr )
		{
			*_resource = entry->resource;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::validResourceType( const ConstString& _name, const ConstString& _type ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );
		
		if( entry == nullptr )
		{
			return false;
		}

		const ConstString & resourceType = entry->resource->getType();

		if( resourceType != _type )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::validResource( const ConstString & _name ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			return false;
		}

		const ResourceReferencePtr & resource = entry->resource;

		bool valid = resource->isValid();

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::lockResource( const ConstString& _name )
	{
		ResourceEntry * entry = this->findResource_( _name );
		
		if( entry == nullptr )
		{
			return false;
		}

		if( entry->isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager getResource: resource '%s' is alredy LOCK!"
				, _name.c_str()
				);

			return false;
		}

		entry->isLocked = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::unlockResource( const ConstString& _name )
	{
		ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			return false;
		}

		if( entry->isLocked == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager getResource: resource '%s' is alredy UNLOCK!"
				, _name.c_str()
				);

			return false;
		}

		entry->isLocked = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReferencePtr ResourceManager::getResource( const ConstString& _name ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' does not exist"
				, _name.c_str()
				);

			return nullptr;
		}

        if( entry->isLocked == true )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' is LOCK!"
                , _name.c_str()
                );

            return nullptr;
        }
		
		const ResourceReferencePtr & resource = entry->resource;

		if( resource->incrementReference() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' '%s' is not compile!"
				, _name.c_str()
                , resource->getType().c_str()
				);

			return nullptr;
		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReferencePtr ResourceManager::getResourceReference( const ConstString & _name ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			LOGGER_WARNING(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' does not exist"
				, _name.c_str()
				);

			return nullptr;
		}
		
        if( entry->isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' is LOCK!"
				, _name.c_str()
				);

			return nullptr;
		}
		
		const ResourceReferencePtr & resource = entry->resource;

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceManager::getResourceType( const ConstString & _name ) const
	{
		const ResourceEntry * entry = this->findResource_( _name );

		if( entry == nullptr )
		{
			LOGGER_WARNING(m_serviceProvider)("ResourceManager::getResourceType: resource '%s' does not exist"
				, _name.c_str()
				);

			return ConstString::none();
		}

		const ResourceReferencePtr & resource = entry->resource;

		const ConstString & type = resource->getType();

		return type;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( Visitor * _visitor ) const
	{
		for( uint32_t i = 0; i != MENGINE_RESOURCE_MANAGER_HASH_SIZE; ++i )
		{
			const TMapResource & resources = m_resources[i];

			for( TMapResource::const_iterator
				it = resources.begin(),
				it_end = resources.end();
			it != it_end;
			++it )
			{
				const ResourceEntry & entry = it->second;

				const ResourceReferencePtr & resource = entry.resource;

				resource->visit( _visitor );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitGroupResources( const ConstString & _category, const ConstString & _group, Visitor * _visitor ) const
	{		
		TResourceCacheKey cache_key = std::make_pair( _category, _group );

		TMapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( cache_key );

		if( it_cache_found == m_resourcesCache.end() )
		{
			return;
		}

		const TVectorResources & resources = it_cache_found->second;

		for( TVectorResources::const_iterator
			it = resources.begin(),
			it_end = resources.end();
		it != it_end;
		++it )
		{
			const ResourceReferencePtr & resource = *it;

			resource->visit( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceManager::getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group ) const
	{
		TResourceCacheKey cache_key = std::make_pair( _category, _group );

		TMapResourceCache::const_iterator it_cache_found = m_resourcesCache.find( cache_key );

		if( it_cache_found == m_resourcesCache.end() )
		{
			return 0U;
		}

		size_t groupMemoryUse = 0U;

		const TVectorResources & resources = it_cache_found->second;
		
		for( TVectorResources::const_iterator
			it = resources.begin(),
			it_end = resources.end();
		it != it_end;
		++it )
		{
			const ResourceReferencePtr & resource = *it;

			size_t memoryUse = resource->getMemoryUse();

			groupMemoryUse += memoryUse;
		}

		return groupMemoryUse;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FResourcesForeachDump
		{
		public:
			FResourcesForeachDump( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider(_serviceProvider)
			{
			}

		private:
			void operator = ( const FResourcesForeachDump & )
			{
			}

		public:
			void operator () ( ResourceEntry * _entry )
			{
				const ResourceReferencePtr & resource = _entry->resource;

				uint32_t count = resource->countReference();

				if( count == 0 )
				{
					return;
				}

				if( resource->isCompile() == false )
				{
					return;
				}

				size_t memoryUse = resource->getMemoryUse();
				float memoryUseMb = (float)(memoryUse)/(1024.f);

				const ConstString & name = _entry->resource->getName();

				LOGGER_ERROR(m_serviceProvider)("Resource %s\n: count - %u memory - %f"
					, name.c_str()
					, count
					, memoryUseMb
					);
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::dumpResources( const String & _tag )
	{
		(void)_tag;

#	ifdef _DEBUG
		LOGGER_ERROR(m_serviceProvider)("Dumping resources... %s"
			, _tag.c_str()
			);

		for( uint32_t i = 0; i != MENGINE_RESOURCE_MANAGER_HASH_SIZE; ++i )
		{
			const TMapResource & resources = m_resources[i];

			for( TMapResource::const_iterator
				it = resources.begin(),
				it_end = resources.end();
			it != it_end;
			++it )
			{
				const ResourceEntry & entry = it->second;

				const ResourceReferencePtr & resource = entry.resource;

				uint32_t count = resource->countReference();

				if( count == 0 )
				{
					return;
				}

				if( resource->isCompile() == false )
				{
					return;
				}

				size_t memoryUse = resource->getMemoryUse();
				float memoryUseMb = (float)(memoryUse) / (1024.f);

				const ConstString & name = entry.resource->getName();

				LOGGER_ERROR( m_serviceProvider )("Resource %s\n: count - %u memory - %f"
					, name.c_str()
					, count
					, memoryUseMb
					);
			}
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceEntry * ResourceManager::findResource_( const ConstString & _name )
	{
		ConstString::hash_type hash = _name.hash();
		uint32_t table = (uint32_t)hash % MENGINE_RESOURCE_MANAGER_HASH_SIZE;
		TMapResource & resources = m_resources[table];

		TMapResource::iterator it_found = resources.find( _name );

		if( it_found == resources.end() )
		{
			return nullptr;
		}

		ResourceEntry & entry = it_found->second;

		return &entry;
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceEntry * ResourceManager::findResource_( const ConstString & _name ) const
	{
		ConstString::hash_type hash = _name.hash();
		uint32_t table = (uint32_t)hash % MENGINE_RESOURCE_MANAGER_HASH_SIZE;
		const TMapResource & resources = m_resources[table];

		TMapResource::const_iterator it_found = resources.find( _name );

		if( it_found == resources.end() )
		{
			return nullptr;
		}

		const ResourceEntry & entry = it_found->second;

		return &entry;
	}
}
