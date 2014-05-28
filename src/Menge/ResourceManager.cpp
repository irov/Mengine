#	include "ResourceManager.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LoaderInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Loadable.h"
#	include "Kernel/ResourceReference.h"

#   include "Consts.h"

#	include "Logger/Logger.h"

#	include "Kernel/ResourceVisitor.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ResourceService, Menge::ResourceServiceInterface, Menge::ResourceManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::~ResourceManager()
	{		
        //_dumpResources();

		m_resourcesCache.clear();

        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it)
        {
            const ResourceEntry & entry = m_resources.get_value(it);

            ResourceReference * resource = entry.resource;

#   ifndef MENGE_MASTER_RELEASE
            size_t refcount = resource->countReference();
            if ( refcount != 0 )
			{
				LOGGER_WARNING(m_serviceProvider)("ResourceManager::~ResourceManager resource %s refcount %d"
					, resource->getName().c_str()
					, refcount
					);

				while( refcount != 0 )
				{
					refcount = resource->decrementReference();
				}
			}
#   endif

            resource->destroy();
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ResourceManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::initialize( size_t _reserved )
	{
		m_resources.reserve( _reserved );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::loadResource( const ConstString & _pakName, const ConstString & _path )
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

            const ConstString & path = meta_include.get_Path();

            if( this->loadResource( _pakName, path ) == false )
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

        size_t resources_size = m_resources.size();
        size_t includes_size = includes_resource.size();
        m_resources.reserve( resources_size + includes_size );

        for( Metacode::Meta_DataBlock::TVectorMeta_Resource::const_iterator
            it = includes_resource.begin(),
            it_end = includes_resource.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_Resource * meta_resource = *it;

            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

#   ifdef _DEBUG
			ResourceReference * has_resource = nullptr;
			if( this->hasResource( name, &has_resource ) == true )
			{
				const ConstString & resource_category = has_resource->getCategory();

				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: path %s already exist resource name '%s' in group '%s' category '%s' ('%s')"
					, _path.c_str()
					, name.c_str()
					, groupName.c_str()
					, _pakName.c_str()
					, resource_category.c_str()
					);

				LOGGER_ERROR(m_serviceProvider)("has resource category '%s' group '%s' name '%s'"
					, has_resource->getCategory().c_str()
					, has_resource->getGroup().c_str()
					, has_resource->getName().c_str()
					);

				//return nullptr;
			}
#   endif

            ResourceReference * resource = 
                this->createResource( _pakName, groupName, name, type );

            if( resource == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: invalid create resource %s:%s name %s type %s"
                    , _pakName.c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                    );

                return false;
            }

            if( resource->loader( meta_resource ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid load"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }
            
#	ifndef MENGE_MASTER_RELEASE
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
    bool ResourceManager::validationResources() const
    {
        LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource validation resource begin (PLEASE WAIT):");
        LOGGER_WARNING(m_serviceProvider)("----------------------------------------------------------------------");

		bool total_successful = true;

        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it )
        {
            const ResourceEntry & entry = m_resources.get_value( it );

			const ResourceReference * resource = entry.resource;

			bool successful = resource->isValid();

			if( successful == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid validation"
					, resource->getName().c_str()
					, resource->getType().c_str()
					);

				LOGGER_WARNING(m_serviceProvider)("======================================================================");
				LOGGER_WARNING(m_serviceProvider)("");

				total_successful = false;
			}
        }

        LOG_SERVICE(m_serviceProvider)->logMessage( Menge::LM_WARNING, 0, "\n", 2 );

        LOGGER_WARNING(m_serviceProvider)("----------------------------------------------------------------------");
        LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource validation resource complete!");

		return total_successful;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
	{
        ResourceReference * resource = PROTOTYPE_SERVICE(m_serviceProvider)
            ->generatePrototypeT<ResourceReference>( CONST_STRING(m_serviceProvider, Resource), _type );

		if( resource == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager createResource: not registered resource type '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

        LOGGER_INFO(m_serviceProvider)("ResourceManager::createResource category %s group %s name %s type %s"
            , _category.c_str()
            , _group.c_str()
            , _name.c_str()
            , _type.c_str()
            );

		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );		

		ResourceEntry entry;
		entry.resource = resource;
		entry.isLocked = false;

        m_resources.insert( entry );

		CategoryGroupKey key;
		key.category = _category;
		key.group = _group;

		TVectorResources * resources;
		if( m_resourcesCache.has( key, &resources ) == false )
		{
			TVectorResources resources;
			resources.reserve( 100 );
			resources.push_back( resource );

			m_resourcesCache.insert( key, resources );
		}
		else
		{
			resources->push_back( resource );
		}
        
		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasResource( const ConstString& _name, ResourceReference ** _resource ) const
	{
		ResourceEntry * entry;
		if( m_resources.has( _name, &entry ) == false )
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
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
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
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
		{
			return false;
		}

		ResourceReference * resource = entry->resource;

		bool valid = resource->isValid();

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::lockResource( const ConstString& _name )
	{
		ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
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
		ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
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
	ResourceReference * ResourceManager::getResource( const ConstString& _name ) const
	{
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
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
		
		ResourceReference * resource = entry->resource;

		if( resource->incrementReference() == 0 )
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
	ResourceReference * ResourceManager::getResourceReference( const ConstString & _name ) const
	{
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
		{
#	ifdef _DEBUG
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' does not exist"
				, _name.c_str()
				);
#	else
			LOGGER_WARNING(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' does not exist"
				, _name.c_str()
				);
#	endif
			return nullptr;
		}
		
        if( entry->isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' is LOCK!"
				, _name.c_str()
				);

			return nullptr;
		}
		
		ResourceReference * resource = entry->resource;

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceManager::getResourceType( const ConstString & _name ) const
	{
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
		{
			return ConstString::none();
		}

		ResourceReference * resource = entry->resource;

		const ConstString & type = resource->getType();

		return type;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( ResourceVisitor * _visitor ) const
	{
        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it )
        {
            const ResourceEntry & entry = m_resources.get_value( it );

            ResourceReference * resource = entry.resource;

			resource->accept( _visitor );
            //_visitor->visit( resource );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitGroupResources( const ConstString & _category, const ConstString & _group, ResourceVisitor * _visitor ) const
	{		
		CategoryGroupKey key;
		key.category = _category;
		key.group = _group;

		TVectorResources * resources;
		if( m_resourcesCache.has( key, &resources ) == false )
		{
			return;
		}

		for( TVectorResources::const_iterator
			it = resources->begin(),
			it_end = resources->end();
		it != it_end;
		++it )
		{
			ResourceReference * resource = *it;

			resource->accept( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const ConstString& _name )
	{
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
		{
			return false;
		}
		
		ResourceReference * ref = entry->resource;

		ref->incrementReference();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const ConstString& _name )
	{
		const ResourceEntry * entry = nullptr;
		if( m_resources.has( _name, &entry ) == false )
		{
			return;
		}

		ResourceReference * ref = entry->resource;

		ref->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::dumpResources( const String & _tag )
	{
#	ifdef _DEBUG
        LOGGER_ERROR(m_serviceProvider)("Dumping resources... %s"
            , _tag.c_str()
            );

		for( TMapResource::iterator 
			it = m_resources.begin()
			, it_end = m_resources.end();
		it != it_end;
		++it )
		{
			const ResourceEntry & entry = m_resources.get_value(it);

			ResourceReference * resource = entry.resource;

			size_t count = resource->countReference();

			if( count == 0 )
			{
				continue;
			}

			if( resource->isCompile() == false )
			{
				continue;
			}

			size_t memoryUse = resource->memoryUse();
            float memoryUseMb = float(memoryUse)/(1024.f*1024.f);

			const ConstString & name = m_resources.get_key(it);

            LOGGER_ERROR(m_serviceProvider)("--> %s : count - %u memory - %f"
                , name.c_str()
                , count
                , memoryUseMb
                );
		}
#	endif
	}
}
