#	include "ResourceManager.h"

#	include "Loadable.h"
#	include "ResourceReference.h"

#	include "LoaderEngine.h"
#	include "Metacode.h"

#	include "ServiceProvider.h"

#	include "LogEngine.h"

#	include "Application.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "ResourceVisitor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
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
	bool ResourceManager::loadResource( const ResourceDesc & _desc )
	{
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LoaderEngine::get()
			->load( _desc.pakName, _desc.path, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				MENGE_LOG_ERROR( "ResourceManager: resource '%s:%S' not found"
					, _desc.pakName.c_str()
					, _desc.path.c_str()
					);
			}
			else
			{
				MENGE_LOG_ERROR( "ResourceManager: Invalid parse resource '%s:%S'"
                    , _desc.pakName.c_str()
                    , _desc.path.c_str()
					);
			}

			return false;
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

            ResourceReference * resource = 
                this->createResource( _desc.pakName, _desc.name, name, type );

            resource->loader( meta_resource );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const ConstString& _category, const ConstString& _group, const ConstString& _name, const ConstString& _type )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find != m_resources.end() )
		{
			MENGE_LOG_ERROR( "ResourceManager createResource: already exist resource name '%s' in group '%s' category '%s'"
				, _name.c_str()
				, _group.c_str()
				, _category.c_str()
				);

			return 0;
		}

		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			MENGE_LOG_ERROR( "ResourceManager createResource: don't register resource type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );
		resource->setType( _type );

		ResourceEntry entry;
		entry.resource = resource;
		entry.isLocked = false;

		m_resources.insert( it_find, std::make_pair( _name, entry ) );

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
			MENGE_LOG_ERROR( "ResourceManager getResource: resource '%s' is alredy LOCK!"
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
			MENGE_LOG_ERROR( "ResourceManager getResource: resource '%s' is alredy UNLOCK!"
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
			MENGE_LOG_ERROR( "ResourceManager::getResource: resource '%s' does not exist"
				, _name.c_str()
				);

			return 0;
		}

		const ResourceEntry & entry = it_find->second;
		if( entry.isLocked == true )
		{
			MENGE_LOG_ERROR( "ResourceManager::getResource: resource '%s' is LOCK!"
				, _name.c_str()
				);

			return 0;
		}
		
		ResourceReference * resource = entry.resource;

		if( resource->incrementReference() == 0 )
		{
			MENGE_LOG_ERROR( "ResourceManager::getResource: resource '%s' is not compile!"
				, _name.c_str()
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
			MENGE_LOG_ERROR( "ResourceManager::getResourceReference: resource '%s' does not exist"
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
			return ConstString::none;
		}

		const ResourceEntry & entry = it_found->second;
		ResourceReference * resource = entry.resource;

		const ConstString & type = resource->getType();

		return type;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( const ConstString & _category, const ConstString & _groupName, ResourceVisitor * _visitor )
	{
		for( TMapResource::iterator
			it = m_resources.begin(),
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			const ResourceEntry & entry = it->second;

			const ConstString & category = entry.resource->getCategory();
			const ConstString & group = entry.resource->getGroup();

			if( category != _category )
			{
				continue;
			}

			if( group != _groupName )
			{
				continue;
			}

			_visitor->visit( entry.resource );
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
		fprintf( file, _tag.c_str() );
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

			fprintf( file, "--> %s : count - %d memory - %f\n"
				, it->first.c_str()
				, count
				, float(memoryUse)/(1024.f*1024.f)
				);
		}

		fclose( file );
#	endif
	}
}
