
#	include "ResourceManager.h"

#	include "Application.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Logger/Logger.h"
#	include "ScriptEngine.h"

#	include <ctime>
#	include "pybind/include/pybind/pybind.hpp"
#	include "RenderEngine.h"
#	include "Game.h"

#	include "Utils/Core/String.h"


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
			it = m_mapResource.begin(),
			it_end = m_mapResource.end();
		it != it_end;
		++it)
		{
			ResourceReference * resource = *it->second;
			resource->destroy();
		}

		for( TMapResourceManagerListenerScript::iterator
			it = m_scriptListeners.begin(),
			it_end = m_scriptListeners.end();
		it != it_end;
		++it )
		{
			ScriptEngine::decref( it->second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::loadResource( const ConstString& _category, const ConstString& _group, const String& _file )
	{
		TResourcePackMap::iterator it_find_pack = m_resourcePackMap.find( _group );
		if( it_find_pack != m_resourcePackMap.end() )
		{
			MENGE_LOG_WARNING( "Warning: Resource group '%s' already exist",
				_group.c_str()
				);

			return false;
		}

		m_currentCategory = _category;
		m_currentGroup = _group;

		m_resourcePackMap.insert( std::make_pair( _group, _category ) );

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentGroup );
		if( it_find == m_resourceCountMap.end() )
		{
			m_resourceCountMap.insert( std::make_pair( m_currentGroup, 0 ) );
		}

		if( LoaderEngine::get()
			->load( _category, _file, this ) == false )
		//if( XmlEngine::get()
		//	->parseXmlFileM( _category, _file, this, &ResourceManager::loaderDataBlock ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse resource '%s'"
				, _file.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::DataBlock, this, &ResourceManager::loaderResource_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::_loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loaderResource_( BinParser * _parser )
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
					this->createResource( name, type );

				if( resource == 0 )
				{
					MENGE_LOG_ERROR( "Don't register resource type '%s'"
						, type.c_str() 
						);

					BIN_SKIP();
				}

				bool registered = this->registerResource( resource );

				if( registered == true )
				{
					BIN_PARSE( resource );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const ConstString& _name, const ConstString& _type )
	{
		ResourceFactoryParam param;
		param.name = _name;
		param.category = m_currentCategory;
		param.group = m_currentGroup;

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentGroup );
		if( it_find != m_resourceCountMap.end() )
		{
			++it_find->second;
		}

		ResourceReference * resource = 
			this->createResourceWithParam( _type, param );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResourceWithParam( const ConstString& _type, const ResourceFactoryParam & _param )
	{
		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			return 0;
		}

		resource->initialize( _param );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::registerResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return false;
		}

		const ConstString& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find == m_mapResource.end() )
		{
			TListResource::iterator it_insert = this->cacheGroupResource_( _resource );

			m_mapResource.insert( std::make_pair( name, it_insert ) );			
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Duplicate resource name '%s' in group '%s' (delete)"
				, _resource->getName().c_str()
				, _resource->getGroup().c_str()
				);

			_resource->destroy(); //Duplicate entry will be deleted now;

			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::TListResource::iterator ResourceManager::cacheGroupResource_( ResourceReference * _resource )
	{
		const ConstString & category = _resource->getCategory();

		TCacheGroupResource::iterator it_find = m_cacheGroupResource.find( category );

		if( it_find == m_cacheGroupResource.end() )
		{
			it_find = m_cacheGroupResource.insert( std::make_pair( category, TListResource() ) ).first;
		}

		TListResource::iterator it_insert = it_find->second.insert( it_find->second.end(), _resource );

		return it_insert;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::unregisterResource( ResourceReference* _resource )
	{
		const ConstString & name = _resource->getName();

		TMapResource::iterator it_resource_find = m_mapResource.find( name );

		if( it_resource_find == m_mapResource.end() )
		{			
			MENGE_LOG_ERROR( "Warning: Unregister unknown resource name '%s' in group '%s' category '%s'"
				, _resource->getName().c_str()
				, _resource->getGroup().c_str()
				, _resource->getCategory().c_str()
				);

			return;
		}

		const ConstString & category = _resource->getCategory();

		TCacheGroupResource::iterator it_category_find = m_cacheGroupResource.find( category );

		if( it_category_find == m_cacheGroupResource.end() )
		{
			MENGE_LOG_ERROR( "Internal Error: Unregister non-cache resource name '%s' in group '%s' category '%s'"
				, _resource->getName().c_str()
				, _resource->getGroup().c_str()
				, _resource->getCategory().c_str()
				);

			return;
		}

		it_category_find->second.erase( it_resource_find->second );
		m_mapResource.erase( it_resource_find );
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResource( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			//MENGE_LOG_INFO( "Warning: resource named '%s' does not exist"
			//	, _name.c_str() );
			return 0;
		}

		ResourceReference * resource = *it_find->second;

		unsigned int inc = resource->incrementReference();

		if( inc == 0 )
		{
			return 0;
		}

		// resource has been loaded
		if( inc == 1 && ( !m_listeners.empty() || !m_scriptListeners.empty() ) )
		{
			for( TListResourceManagerListener::iterator 
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it)
			{
				(*it)->onResourceLoaded( _name );
			}

			for( TMapResourceManagerListenerScript::iterator 
				it = m_scriptListeners.begin(),
				it_end = m_scriptListeners.end();
			it != it_end;
			++it)
			{
				String nameAnsi = Application::get()
					->utf8ToAnsi( _name.str() );

				ScriptEngine::get()
					->callFunction( it->second, "(s)", nameAnsi.c_str() );
			}

		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::releaseResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const ConstString & name = _resource->getName();
		

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find != m_mapResource.end() && (*it_find->second) != NULL )
		{
			//unsigned int ref_debug = _resource->countReference();
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
					(*it)->onResourceUnLoaded();
				}

				/*for( TMapResourceManagerListenerScript::iterator it = m_scriptListeners.begin();
					it != m_scriptListeners.end();
					it++)
				{
					PyObject * result = 
						Holder<ScriptEngine>::get()
						->callFunction( it->second, "()", it->first );
				}*/
			}

			//Holder<ProfilerEngine>::get()->removeResourceToProfile(name);
		}
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
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return;
		}

		ResourceReference * ref = *it_find->second;
		ref->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::addListener( ResourceManagerListener* _listener )
	{
		m_listeners.push_back( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::addListener( PyObject* _listener )
	{
		if( ScriptEngine::get()
			->hasModuleFunction( _listener, ("onHandleResourceLoaded") ) == false )
		{
			return;
		}

		PyObject * event = ScriptEngine::get()
			->getModuleFunction( _listener, ("onHandleResourceLoaded") );

		if( event == 0 )
		{
			return;
		}

		m_scriptListeners.insert( std::make_pair( _listener, event ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::removeListener( ResourceManagerListener* _listener )
	{
		m_listeners.remove( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::removeListener( PyObject* _listener )
	{
		m_scriptListeners.erase( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceUnload( const ConstString& _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return;
		}

		ResourceReference * ref = *it_find->second;

		while( ref->countReference() )
		{
			ref->decrementReference();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileCompile( const ConstString& _resourceFile )
	{
		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( _resourceFile );

		if( it_found == m_cacheGroupResource.end() )
		{
			return;
		}

		for( TListResource::iterator 
			it = it_found->second.begin(),
			it_end = it_found->second.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			getResource( name );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileRelease( const ConstString& _resourceFile )
	{
		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( _resourceFile );

		if( it_found == m_cacheGroupResource.end() )
		{
			return;
		}

		for( TListResource::iterator 
			it = it_found->second.begin(),
			it_end = it_found->second.end();
		it != it_end;
		++it )
		{			
			(*it)->decrementReference();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileUnload( const ConstString& _resourceFile )
	{
		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( _resourceFile );

		if( it_found == m_cacheGroupResource.end() )
		{
			return;
		}

		for( TListResource::iterator 
			it = it_found->second.begin(),
			it_end = it_found->second.end();
		it != it_end;
		++it )
		{
			while( (*it)->decrementReference() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::dumpResources( const String & _tag )
	{
		FILE* file = fopen( "ResourceDump.log", "a" );
		fprintf( file, "Dumping resources... ");
		fprintf( file, _tag.c_str() );
		fprintf( file, "\n" );

		for( TMapResource::iterator it = m_mapResource.begin()
			, it_end = m_mapResource.end()
			; it != it_end
			; it++ )
		{
			ResourceReference * ref = *it->second;

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
	void ResourceManager::visitResources( ResourceVisitor * _visitor, const ConstString & _resourceFile )
	{
		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( _resourceFile );

		if( it_found == m_cacheGroupResource.end() )
		{
			return;
		}

		for( TListResource::iterator 
			it = it_found->second.begin(),
			it_end = it_found->second.end();
		it != it_end;
		++it )
		{
			(*it)->accept( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceManager::getResourceCount( const ConstString& _resourceFile )
	{
		TResourceCountMap::iterator it_find = m_resourceCountMap.find( _resourceFile );
		if( it_find != m_resourceCountMap.end() )
		{
			return it_find->second;
		}

		MENGE_LOG_ERROR( "Warning: (ResourceManager::getResourceCount) Resource File '%s' not exist"
			, _resourceFile.c_str() 
			);

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceManager::getCategoryResource( const ConstString& _group ) const
	{
		TResourcePackMap::const_iterator it_find = m_resourcePackMap.find( _group );
		if( it_find != m_resourcePackMap.end() )
		{
			return it_find->second;
		}

		return Utils::emptyConstString();
	}
	//////////////////////////////////////////////////////////////////////////
}
