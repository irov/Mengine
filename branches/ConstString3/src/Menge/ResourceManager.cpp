#	include "ResourceManager.h"

#	include "Loadable.h"
#	include "ResourceReference.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "LogEngine.h"

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
			MENGE_LOG_INFO( "ResourceManager getResource: resource '%s' is alredy LOCK!"
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
			MENGE_LOG_INFO( "ResourceManager getResource: resource '%s' is alredy UNLOCK!"
				, _name.c_str() 
				);

			return false;
		}

		entry.isLocked = false;

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

		const ResourceEntry & entry = it_find->second;
		if( entry.isLocked == true )
		{
			MENGE_LOG_INFO( "ResourceManager getResource: resource '%s' is LOCK!"
				, _name.c_str() 
				);

			return 0;
		}
		
		ResourceReference * resource = entry.resource;

		if( resource->incrementReference() == 0 )
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
