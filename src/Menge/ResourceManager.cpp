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
	namespace
	{
		class FResourcesForeachDestroy
		{
		public:
			FResourcesForeachDestroy( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider(_serviceProvider)
			{
			}

		private:
			void operator = ( const FResourcesForeachDestroy & )
			{
			}

		public:
			void operator() ( ResourceEntry * _entry )
			{
				const ResourceReferencePtr & resource = _entry->resource;

#   ifndef MENGINE_MASTER_RELEASE
				uint32_t refcount = resource->countReference();
				if ( refcount != 0 )
				{
					LOGGER_WARNING(m_serviceProvider)("ResourceManager::~ResourceManager resource %s refcount %d"
						, resource->getName().c_str()
						, refcount
						);
				}
#   endif
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::_finalize()
	{
		m_resourcesCache.clear();

		FResourcesForeachDestroy rfd(m_serviceProvider);
		m_resources.foreach( rfd );

		m_resources.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::loadResources( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
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

			if( this->loadResources( _locale, _pakName, path ) == false )
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
            
#	ifndef MENGINE_MASTER_RELEASE
			if( resource->convert() == false )
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

		ResourceEntry * entry = m_resources.create();
				
		entry->resource = resource;
		entry->isLocked = false;

		ResourceEntry * insert_entry;
		bool successful = m_resources.insert( entry, &insert_entry );

		ResourceCacheEntry * resourceCacheEntry;
		if( m_resourcesCache.has( _category, _group, &resourceCacheEntry ) == false )
		{
			resourceCacheEntry = m_resourcesCache.create();

			resourceCacheEntry->category = _category;
			resourceCacheEntry->group = _group;

			resourceCacheEntry->resources.push_back( resource );

			m_resourcesCache.insert(resourceCacheEntry, nullptr );
		}
		else
		{
			resourceCacheEntry->resources.push_back( resource );
		}

		if( successful == false )
		{
			const ConstString & insert_category = insert_entry->resource->getCategory();
			const ConstString & insert_group = insert_entry->resource->getGroup();

			ResourceCacheEntry * resourceCacheEntryFound = m_resourcesCache.find( insert_category, insert_group );

			TVectorResources::iterator it_found = std::remove(
				resourceCacheEntryFound->resources.begin(),
				resourceCacheEntryFound->resources.end(),
				insert_entry->resource );

			resourceCacheEntryFound->resources.erase( it_found );

			insert_entry->resource = resource;
			insert_entry->isLocked = false;

			entry->destroy();
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

		if( m_resources.erase( name ) == false )
		{
			return false;
		}

		const ConstString & category = _resource->getCategory();
		const ConstString & group = _resource->getGroup();

		ResourceCacheEntry * resourceCacheEntry;
		if( m_resourcesCache.has( category, group, &resourceCacheEntry ) == false )
		{
			return false;
		}

		TVectorResources::iterator it_found = std::remove(
			resourceCacheEntry->resources.begin(),
			resourceCacheEntry->resources.end(),
			_resource );

		resourceCacheEntry->resources.erase( it_found );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasResource( const ConstString& _name, ResourceReferencePtr * _resource ) const
	{
		const ResourceEntry * entry = m_resources.find( _name );
		
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
		const ResourceEntry * entry = m_resources.find( _name );
		
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
		const ResourceEntry * entry = m_resources.find( _name );

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
		ResourceEntry * entry = m_resources.find( _name );
		
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
		ResourceEntry * entry = m_resources.find( _name );

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
		const ResourceEntry * entry = m_resources.find( _name );

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
		const ResourceEntry * entry = m_resources.find( _name );

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
		const ResourceEntry * entry = m_resources.find( _name );

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
	namespace
	{
		class FResourcesForeachVisit
		{
		public:
			FResourcesForeachVisit( Visitor * _visitor )
				: m_visitor(_visitor)
			{
			}

		private:
			void operator = ( const FResourcesForeachVisit & )
			{
			}

		public:
			void operator () ( ResourceEntry * _entry )
			{
				const ResourceReferencePtr & resource = _entry->resource;

				resource->visit( m_visitor );
			}

		protected:
			Visitor * m_visitor;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( Visitor * _visitor ) const
	{
		FResourcesForeachVisit rfv(_visitor);
		m_resources.foreach( rfv );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitGroupResources( const ConstString & _category, const ConstString & _group, Visitor * _visitor ) const
	{		
		const ResourceCacheEntry * resourceCacheEntry;
		if( m_resourcesCache.has( _category, _group, &resourceCacheEntry ) == false )
		{
			return;
		}

		const TVectorResources & resources = resourceCacheEntry->resources;

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
		const ResourceCacheEntry * resourceCacheEntry;
		if( m_resourcesCache.has( _category, _group, &resourceCacheEntry ) == false )
		{
			return 0U;
		}

		size_t groupMemoryUse = 0U;

		const TVectorResources & resources = resourceCacheEntry->resources;

		for( TVectorResources::const_iterator
			it = resources.begin(),
			it_end = resources.end();
		it != it_end;
		++it )
		{
			const ResourceReferencePtr & resource = *it;

			size_t memoryUse = resource->memoryUse();

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

				size_t memoryUse = resource->memoryUse();
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

		FResourcesForeachDump rfd(m_serviceProvider);
		m_resources.foreach( rfd );
#	endif
	}
}
