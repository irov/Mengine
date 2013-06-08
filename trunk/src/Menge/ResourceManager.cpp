#	include "ResourceManager.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LoaderInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Loadable.h"
#	include "Kernel/ResourceReference.h"

#   include "Consts.h"

#	include "Metacode.h"

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

        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it)
        {
            const ResourceEntry & entry = it->second;

            ResourceReference * resource = entry.resource;
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
	bool ResourceManager::loadResource( const ResourceDesc & _desc )
	{
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LOADER_SERVICE(m_serviceProvider)->load( _desc.pakName, _desc.path, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "ResourceManager::loadResource: resource '%s:%s' not found"
					, _desc.pakName.c_str()
					, _desc.path.c_str()
					);
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)( "ResourceManager::loadResource: Invalid parse resource '%s:%s'"
                    , _desc.pakName.c_str()
                    , _desc.path.c_str()
					);
			}

			return false;
		}

        ConstString groupName;
        datablock.swap_Name( groupName );

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

                LOGGER_ERROR(m_serviceProvider)("ResourceManager createResource: already exist resource name '%s' in group '%s' category '%s' ('%s')"
                    , name.c_str()
                    , groupName.c_str()
                    , _desc.pakName.c_str()
                    , resource_category.c_str()
                    );

                return nullptr;
            }
#   endif

            ResourceReference * resource = 
                this->createResource( _desc.pakName, groupName, name, type );

            if( resource == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource: invalid create resource %s:%s name %s type %s"
                    , _desc.pakName.c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                    );

                return false;
            }

            if( resource->_loader( meta_resource ) == false )
            {
                LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid load"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }
            
            if( resource->convert() == false )
            {
                LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid convert"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::validationResources() const
    {
        LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource validation resource begin (PLEASE WAIT):");
        LOGGER_WARNING(m_serviceProvider)("----------------------------------------------------------------------");

        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it )
        {
            const ResourceReference * resource = it->second.resource;

            if( resource->isValid() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceManager::loadResource %s type [%s] invalid validation"
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                    );
            }
        }

        LOG_SERVICE(m_serviceProvider)->logMessage( Menge::LM_WARNING, 0, "\n", 2 );

        LOGGER_WARNING(m_serviceProvider)("----------------------------------------------------------------------");
        LOGGER_WARNING(m_serviceProvider)("ResourceManager::loadResource validation resource complete!");
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

		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );		

		ResourceEntry entry;
		entry.resource = resource;
		entry.isLocked = false;

        m_resources.insert( std::make_pair(_name, entry) );
        
		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasResource( const ConstString& _name, ResourceReference ** _resource ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

        if( _resource != nullptr )
        {
            *_resource = it_find->second.resource;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::validResourceType( const ConstString& _name, const ConstString& _type ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		const ResourceEntry & entry = it_find->second;

		const ConstString & resourceType = entry.resource->getType();

		if( resourceType != _type )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::validResource( const ConstString & _name ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		const ResourceEntry & entry = it_find->second;

		ResourceReference * resource = entry.resource;

		bool valid = resource->isValid();

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::lockResource( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		ResourceEntry & entry = it_find->second;

		if( entry.isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager getResource: resource '%s' is alredy LOCK!"
				, _name.c_str()
				);

			return false;
		}

		entry.isLocked = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::unlockResource( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		ResourceEntry & entry = it_find->second;

		if( entry.isLocked == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager getResource: resource '%s' is alredy UNLOCK!"
				, _name.c_str()
				);

			return false;
		}

		entry.isLocked = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResource( const ConstString& _name ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' does not exist"
				, _name.c_str()
				);

			return nullptr;
		}

		const ResourceEntry & entry = it_find->second;

		if( entry.isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' is LOCK!"
				, _name.c_str()
				);

			return nullptr;
		}
		
		ResourceReference * resource = entry.resource;

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
	ResourceReference * ResourceManager::getResourceReference( const ConstString& _name ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResourceReference: resource '%s' does not exist"
				, _name.c_str()
				);

			return nullptr;
		}

		const ResourceEntry & entry = it_find->second;
		
        if( entry.isLocked == true )
		{
			return nullptr;
		}
		
		ResourceReference * resource = entry.resource;

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceManager::getResourceType( const ConstString & _name ) const
	{
		TMapResource::const_iterator it_found = m_resources.find( _name );

		if( it_found == m_resources.end() )
		{
			return ConstString::none();
		}

		const ResourceEntry & entry = it_found->second;

		ResourceReference * resource = entry.resource;

		const ConstString & type = resource->getType();

		return type;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( const ConstString & _category, const ConstString & _group, ResourceVisitor * _visitor ) const
	{
        for( TMapResource::const_iterator
            it = m_resources.begin(),
            it_end = m_resources.end();
        it != it_end;
        ++it )
        {
            const ResourceEntry & entry = it->second;

            ResourceReference * resource = entry.resource;

            const ConstString & category = resource->getCategory();

            if( category != _category )
            {
                continue;
            }

            const ConstString & group = resource->getGroup();

            if( group != _group )
            {
                continue;
            }

            _visitor->visit( resource );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		const ResourceEntry & entry = it_find->second;

		ResourceReference * ref = entry.resource;

		ref->incrementReference();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return;
		}

		const ResourceEntry & entry = it_find->second;

		ResourceReference * ref = entry.resource;

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
			const ResourceEntry & entry = it->second;

			ResourceReference * resource = entry.resource;

			unsigned int count = resource->countReference();

			if( count == 0 )
			{
				continue;
			}

			if( resource->isCompile() == false )
			{
				continue;
			}

			unsigned int memoryUse = resource->memoryUse();
            float memoryUseMb = float(memoryUse)/(1024.f*1024.f);

            LOGGER_ERROR(m_serviceProvider)("--> %s : count - %u memory - %f"
                , it->first.c_str()
                , count
                , memoryUseMb
                );
		}
#	endif
	}
}
