#	include "ResourceManager.h"

#	include "Loadable.h"
#	include "ResourceReference.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Logger/Logger.h"

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
			ResourceReference * resource = it->second;
			resource->destroy();
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
							MENGE_LOG_ERROR( "Don't register resource type '%s'"
								, type.c_str() 
								);

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
	bool ResourceManager::loadResource( const ConstString& _category, const ConstString& _group, const String& _file )
	{
		LoadableResourceManager loadable(this, _category, _group);

		bool exist = false;
		if( LoaderEngine::get()
			->load( _category, _file, &loadable, exist ) == false )
		{
			MENGE_LOG_ERROR( "ResourceManager: Invalid parse resource '%s'"
				, _file.c_str() 
				);

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

			return NULL;
		}

		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			return 0;
		}

		resource->setCategory( _category );
		resource->setGroup( _group );
		resource->setName( _name );
		resource->setType( _type );

		TListResource & group = this->getGroupResources_( _category, _group );
		group.push_back( resource );

		m_resources.insert( it_find, std::make_pair( _name, resource ) );

		return resource;
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

		ResourceReference * resource = it_find->second;

		if( this->increfResource( resource ) == false )
		{
			return 0;
		}

		return resource;
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
		ResourceReference * resource = getResource( _name );
		
		return resource != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_resources.find( _name );

		if( it_find == m_resources.end() )
		{
			return;
		}

		ResourceReference * ref = it_find->second;
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
			const ConstString & name = (*it)->getName();
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
			(*it)->decrementReference();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::dumpResources( const String & _tag )
	{
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
			ResourceReference * ref = it->second;

			if( ref->countReference() )
			{
				fprintf( file, "--> %s : %d\n"
					, it->first.c_str()
					, ref->countReference() 
					);
			}
		}

		fclose( file );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( ResourceVisitor * _visitor, const ConstString& _category, const ConstString& _group )
	{
		TListResource & groupResources = this->getGroupResources_( _category, _group );

		for( TListResource::iterator 
			it = groupResources.begin(),
			it_end = groupResources.end();
		it != it_end;
		++it )
		{
			(*it)->accept( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceManager::getResourceCount( const ConstString& _category, const ConstString& _group )
	{
		TListResource & groupResources = this->getGroupResources_( _category, _group );

		return groupResources.size();
	}
}
