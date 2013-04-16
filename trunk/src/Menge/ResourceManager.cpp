#	include "ResourceManager.h"

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LoaderInterface.h"

#	include "Kernel/Loadable.h"
#	include "Kernel/ResourceReference.h"

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
        : m_serviceProvider(NULL)
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

        FactoryManager::setServiceProvider( m_serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ResourceManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceManager::registerResourceFactory( const ConstString & _type, Factory * _factory )
    {
        this->registerFactory( _type, _factory );
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

        for( Metacode::Meta_DataBlock::TVectorMeta_Resource::const_iterator
            it = includes_resource.begin(),
            it_end = includes_resource.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_Resource * meta_resource = *it;

            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            ResourceReference * resource = 
                this->createResource( _desc.pakName, groupName, name, type );

            if( resource == NULL )
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
                LOGGER_WARNING(m_serviceProvider)("ResourceManager::createResource %s type [%s] invalid load"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }
            
            if( resource->convert() == false )
            {
                LOGGER_WARNING(m_serviceProvider)("ResourceManager::createResource %s type [%s] invalid convert"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }

#       ifdef _DEBUG
            if( resource->isValid() == false )
            {
                LOGGER_WARNING(m_serviceProvider)("ResourceManager::createResource %s type [%s] invalid validation"
                    , name.c_str()
                    , type.c_str()
                    );

                continue;
            }
#       endif
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find != m_resources.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager createResource: already exist resource name '%s' in group '%s' category '%s' ('%s')"
				, _name.c_str()
				, _group.c_str()
				, _category.c_str()
                , it_find->second.resource->getCategory().c_str()
				);

			return 0;
		}

		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager createResource: not registered resource type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );
		resource->setType( _type );

        resource->setServiceProvider( m_serviceProvider );

		ResourceEntry entry;
		entry.resource = resource;
		entry.isLocked = false;

		m_resources.insert( it_find, std::make_pair( _name, entry ) );

        TVectorResource & resources = m_cacheResource[_category][_group];
        resources.push_back( resource );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasResource( const ConstString& _name ) const
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
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

			return 0;
		}

		const ResourceEntry & entry = it_find->second;
		if( entry.isLocked == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' is LOCK!"
				, _name.c_str()
				);

			return 0;
		}
		
		ResourceReference * resource = entry.resource;

		if( resource->incrementReference() == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceManager::getResource: resource '%s' '%s' is not compile!"
				, _name.c_str()
                , resource->getType().c_str()
				);

			return 0;
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

			return 0;
		}

		const ResourceEntry & entry = it_find->second;
		
        if( entry.isLocked == true )
		{
			return 0;
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
	void ResourceManager::visitResources( const ConstString & _category, const ConstString & _groupName, ResourceVisitor * _visitor ) const
	{
        TMapCategoryCacheResource::const_iterator it_category_found = m_cacheResource.find( _category );

        if( it_category_found == m_cacheResource.end() )
        {
            return;
        }

        const TMapGroupCacheResource & cacheGroup = it_category_found->second;

        TMapGroupCacheResource::const_iterator it_group_found = cacheGroup.find( _groupName );

        if( it_group_found == cacheGroup.end() )
        {
            return;
        }

        const TVectorResource & resources = it_group_found->second;

		for( TVectorResource::const_iterator
			it = resources.begin(),
			it_end = resources.end();
		it != it_end;
		++it )
		{
			ResourceReference * resource = *it;

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
		FILE* file = fopen( "ResourceDump.log", "a" );

		if( file == 0 )
		{
			return;
		}

		fprintf( file, "Dumping resources... ");
		fprintf( file, "%s", _tag.c_str() );
		fprintf( file, "\n" );

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

			fprintf( file, "--> %s : count - %u memory - %f\n"
				, it->first.c_str()
				, count
				, float(memoryUse)/(1024.f*1024.f)
				);
		}

		fclose( file );
#	endif
	}
}
