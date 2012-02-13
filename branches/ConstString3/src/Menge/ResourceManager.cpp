#	include "ResourceManager.h"

#	include "Loadable.h"
#	include "ResourceReference.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "LogEngine.h"

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

		for( TMapResource::iterator
			it = m_resources.begin(),
			it_end = m_resources.end();
		it != it_end;
		++it)
		{
			ResourceEntry * entry = it->second;
			ResourceReference * resource = entry->resource;
			resource->destroy();
			delete entry;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class LoadableResourceManager
			: public Loadable
		{
		public:
			LoadableResourceManager( ResourceManager * _resourceMgr, const ConstString & _category, const ConstString & _group )
				: m_resourceMgr(_resourceMgr)
				, m_category(_category)
				, m_group(_group)
			{
			}

		protected:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE_PARSE_METHOD( Protocol::DataBlock, this, &LoadableResourceManager::loaderResource_ );
				}
			}

		private:
			void loaderResource_( BinParser * _parser )
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE( Protocol::Resource )
					{
						ConstString name;
						ConstString type;

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::Resource_Name, name );
							BIN_CASE_ATTRIBUTE( Protocol::Resource_Type, type );
						}

						ResourceReference * resource = 
							m_resourceMgr->createResource( m_category, m_group, name, type );

						if( resource == 0 )
						{
							BIN_SKIP();
						}

						BIN_PARSE( resource );
					}
				}
			}

		protected:
			ResourceManager * m_resourceMgr;
			ConstString m_category;
			ConstString m_group;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::loadResource( const ConstString & _name, const ResourceDesc & _desc )
	{
		String xml_path = _desc.path;
		xml_path += "/";
		xml_path += Helper::to_str(_name);

		LoadableResourceManager loadable(this, _desc.pak, _name);

		bool exist = false;
		if( LoaderEngine::get()
			->load( _desc.pak, xml_path, &loadable, exist ) == false )
		{
			if( exist == false )
			{
				MENGE_LOG_ERROR( "ResourceManager: resource '%s:%s' not found"
					, _name.c_str()
					, xml_path.c_str()
					);
			}
			else
			{
				MENGE_LOG_ERROR( "ResourceManager: Invalid parse resource '%s:%s'"
					, _name.c_str()
					, xml_path.c_str()
					);
			}

			return false;
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

		ResourceEntry * entry = new ResourceEntry();
		entry->resource = resource;
		entry->isLocked = false;

		TListResource & group = this->getGroupResources_( _category, _group );
		group.push_back( entry );

		m_resources.insert( it_find, std::make_pair( _name, entry ) );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::hasGroupResources_( const ConstString& _category, const ConstString& _group ) const
	{
		TCacheCategoryResources::const_iterator it_found_category = m_cacheResources.find(_category);

		if( it_found_category == m_cacheResources.end() )
		{
			return false;
		}

		const TCacheGroupResources & groupResources = it_found_category->second;

		TCacheGroupResources::const_iterator it_found_group = groupResources.find( _group );

		if( it_found_group == groupResources.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::TListResource & ResourceManager::getGroupResources_( const ConstString& _category, const ConstString& _group )
	{
		TCacheCategoryResources::iterator it_found_category = m_cacheResources.find(_category);

		if( it_found_category == m_cacheResources.end() )
		{
			it_found_category = m_cacheResources.insert( std::make_pair( _category, TCacheGroupResources() ) ).first;
		}

		TCacheGroupResources & groupResources = it_found_category->second;

		TCacheGroupResources::iterator it_found_group = groupResources.find(_group);

		if( it_found_group == groupResources.end() )
		{
			it_found_group = groupResources.insert( std::make_pair( _group, TListResource() ) ).first;
		}

		TListResource & resources = it_found_group->second;

		return resources;
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

		ResourceEntry * resourceEntry = it_find->second;
		const ConstString & resourceType = resourceEntry->resource->getType();
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

		ResourceEntry * resourceEntry = it_find->second;
		ResourceReference * resource = resourceEntry->resource;

		bool valid = resource->isValid();

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::lockResource( const ConstString& _name, bool _lock )
	{
		TMapResource::const_iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return;
		}

		ResourceEntry * resourceEntry = it_find->second;
		resourceEntry->isLocked = _lock;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResource( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			MENGE_LOG_INFO( "ResourceManager getResource: resource '%s' does not exist"
				, _name.c_str() 
				);

			return 0;
		}

		ResourceEntry * resourceEntry = it_find->second;
		if( resourceEntry->isLocked == true )
		{
			return 0;
		}
		
		ResourceReference * resource = resourceEntry->resource;

		if( this->increfResource( resource ) == false )
		{
			return 0;
		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResourceReference( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			MENGE_LOG_INFO( "ResourceManager getResource: resource '%s' does not exist"
				, _name.c_str() 
				);

			return 0;
		}

		ResourceEntry * resourceEntry = it_find->second;
		if( resourceEntry->isLocked == true )
		{
			return 0;
		}
		
		ResourceReference * resource = resourceEntry->resource;
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

		ResourceEntry * resourceEntry = it_found->second;
		ResourceReference * resource = resourceEntry->resource;

		const ConstString & type = resource->getType();

		return type;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::increfResource( ResourceReference * _resource )
	{
		unsigned int inc = _resource->incrementReference();

		if( inc == 0 )
		{
			return false;
		}
		
		// resource has been loaded
		if( inc == 1 && m_listeners.empty() ) 
		{
			const ConstString & name = _resource->getName();

			for( TListResourceManagerListener::iterator 
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it)
			{
				(*it)->onResourceLoaded( name );
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::releaseResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return false;
		}

		const ConstString & name = _resource->getName();

		TMapResource::iterator it_find = m_resources.find( name );

		if( it_find == m_resources.end() )
		{
			MENGE_LOG_INFO( "ResourceManager releaseResource: resource '%s' not exist"
				, name.c_str() 
				);

			return false;	
		}


		unsigned int inc = _resource->decrementReference();

		
		// resource has been unloaded
		if( inc == 0 && m_listeners.size() ) 
		{
			for( TListResourceManagerListener::iterator 
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it )
			{
				(*it)->onResourceUnLoaded( name );
			}
		}

		//Holder<ProfilerEngine>::get()->removeResourceToProfile(name);

		return inc == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return false;
		}

		ResourceEntry * resourceEntry = it_find->second;
		ResourceReference * ref = resourceEntry->resource;
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

		ResourceEntry * resourceEntry = it_find->second;
		ResourceReference * ref = resourceEntry->resource;
		ref->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::addListener( ResourceManagerListener* _listener )
	{
		m_listeners.push_back( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::removeListener( ResourceManagerListener* _listener )
	{
		m_listeners.remove( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileCompile( const ConstString& _category, const ConstString& _group )
	{
		TListResource & groupResources = this->getGroupResources_( _category, _group );

		for( TListResource::iterator 
			it = groupResources.begin(),
			it_end = groupResources.end();
		it != it_end;
		++it )
		{
			ResourceEntry * resourceEntry = (*it);
			ResourceReference * reference = resourceEntry->resource;
			const ConstString & name = reference->getName();
			getResource( name );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileRelease( const ConstString& _category, const ConstString& _group )
	{
		TListResource & groupResources = this->getGroupResources_( _category, _group );

		for( TListResource::iterator 
			it = groupResources.begin(),
			it_end = groupResources.end();
		it != it_end;
		++it )
		{
			ResourceEntry * resourceEntry = (*it);
			ResourceReference * reference = resourceEntry->resource;
			reference->decrementReference();
		}
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
			ResourceEntry * resourceEntry = it->second;
			ResourceReference * resource = resourceEntry->resource;

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
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::visitResources( const ConstString& _category, const ConstString& _group, ResourceVisitor * _visitor )
	{
		if( this->hasGroupResources_( _category, _group ) == false )
		{
			return false;
		}

		TListResource & groupResources = this->getGroupResources_( _category, _group );

		for( TListResource::iterator 
			it = groupResources.begin(),
			it_end = groupResources.end();
		it != it_end;
		++it )
		{
			ResourceEntry * resourceEntry = (*it);
			ResourceReference * reference = resourceEntry->resource;
			reference->accept( _visitor );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceManager::getResourceCount( const ConstString& _category, const ConstString& _group )
	{
		TListResource & groupResources = this->getGroupResources_( _category, _group );

		return groupResources.size();
	}

}
