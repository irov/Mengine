#	include "ResourceManager.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"

#	include "LogEngine.h"
#	include "ScriptEngine.h"
#	include "ProfilerEngine.h"

#	include "NodeFactory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::~ResourceManager()
	{
		for( TMapResource::iterator
			it = m_mapResource.begin(),
			it_end = m_mapResource.end();
		it != it_end;
		++it)
		{
			it->second->release();
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loadResource( const String& _category, const String& _file, const String& _group )
	{
		m_currentCategory = _category;
		m_currentGroup = _group;
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &ResourceManager::loaderDataBlock ) == false )
		{
			MENGE_LOG( MENGE_TEXT("Error: Invalid parse resource '%s' \n")
				,_file.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loaderDataBlock( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("DataBlock") )
			{
				XML_PARSE_ELEMENT( this, &ResourceManager::loaderResource );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loaderResource( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("Resource") )
			{
				String name;
				String type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( MENGE_TEXT("Name"), name );
					XML_CASE_ATTRIBUTE( MENGE_TEXT("Type"), type );
				}

				ResourceReference * resource = createResource( name, type );

				if( resource == 0 )
				{
					MENGE_LOG( MENGE_TEXT("Don't register resource type '%s'\n")
						,type.c_str() );
					continue;
				}

				registerResource( resource );

				XML_PARSE_ELEMENT( resource, &ResourceReference::loader );
			}		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const String& _name, const String& _type )
	{
		ResourceFactoryParam param;
		param.name = _name;
		param.category = m_currentCategory;
		param.group = m_currentGroup;
		return TFactoryResource::generate( _type, param );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::registerResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const String& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find == m_mapResource.end() )
		{
			m_mapResource.insert( std::make_pair( name, _resource ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::unregisterResource( ResourceReference* _resource )
	{
		const String& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find != m_mapResource.end() )
		{
			m_mapResource.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResource( const String& _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			MENGE_LOG( MENGE_TEXT( "Warning: resource named \"%s\" does not exist" ), _name.c_str() );
			return 0;
		}

		ResourceReference * resource = it_find->second;

		unsigned int inc = resource->incrementReference();

		// resource has been loaded
		if( inc == 1 && ( !m_listeners.empty() || !m_scriptListeners.empty() ) )
		{
			for( TListResourceManagerListener::iterator it = m_listeners.begin();
				it != m_listeners.end();
				it++)
			{
				(*it)->onResourceLoaded( _name );
			}

			for( TMapResourceManagerListenerScript::iterator it = m_scriptListeners.begin();
				it != m_scriptListeners.end();
				it++)
			{
				Holder<ScriptEngine>::hostage()
					->callFunction( it->second, "(O)", pybind::ptr( _name ) );
			}

		}
		else if( inc == 0 )
		{
			return 0;
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

		const String& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find != m_mapResource.end() )
		{
			unsigned int inc = _resource->decrementReference();
			// resource has been unloaded
			if( inc == 0 && m_listeners.size() )
			{
				for( TListResourceManagerListener::iterator it = m_listeners.begin();
					it != m_listeners.end();
					it++)
				{
					(*it)->onResourceUnLoaded();
				}

				/*for( TMapResourceManagerListenerScript::iterator it = m_scriptListeners.begin();
					it != m_scriptListeners.end();
					it++)
				{
					PyObject * result = 
						Holder<ScriptEngine>::hostage()
						->callFunction( it->second, "()", it->first );
				}*/
			}

			Holder<ProfilerEngine>::hostage()->removeResourceToProfile(name);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const String& _name )
	{
		ResourceReference * resource = getResource( _name );
		
		return resource != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const String& _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
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
	void ResourceManager::addListener( PyObject* _listener )
	{
		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _listener, ("onHandleResourceLoaded") ) == false )
		{
			return;
		}

		PyObject * event = Holder<ScriptEngine>::hostage()
			->getModuleFunction( _listener, ("onHandleResourceLoaded") );

		if( event == 0 )
		{
			return;
		}

		ScriptEngine::incref( event );

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
	class XmlResourceLoaderListener
		: public XmlElementListener
	{
	public:
		XmlResourceLoaderListener( ResourceReference ** _externalResource, ResourceManager* _resourceMgr )
			: m_externalResource( _externalResource )
			, m_resourceManager( _resourceMgr )
		{
		}

	public:
		void parseXML( XmlElement * _xml ) override
		{
			m_resourceManager->loaderResource( _xml );
		}

	protected:
		ResourceReference ** m_externalResource;	
		ResourceManager* m_resourceManager;
	};
	//////////////////////////////////////////////////////////////////////////
	ResourceReference* ResourceManager::createResourceFromXml( const String& _xml )
	{
		ResourceReference* resource = 0;

		XmlResourceLoaderListener * resourceLoader = new XmlResourceLoaderListener( &resource, this );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlString( _xml, resourceLoader ) == false )
		{
			MENGE_LOG( MENGE_TEXT("Invalid parse external node `%s`\n")
				,_xml.c_str() );

			return 0;
		}

		if( resource == 0 )
		{
			MENGE_LOG( MENGE_TEXT("This xml file `%s` have invalid external node format\n")
				,_xml.c_str() );
		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceUnload( const String& _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return;
		}

		ResourceReference * ref = it_find->second;
		while( ref->countReference() )
		{
			ref->decrementReference();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileCompile( const String& _resourceFile )
	{
		for( TMapResource::iterator it = m_mapResource.begin(), it_end = m_mapResource.end();
			it != it_end;
			it++ )
		{
			const ResourceFactoryParam& params = it->second->getFactoryParams();
			if( params.group == _resourceFile )
			{
				getResource( params.name );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileRelease( const String& _resourceFile )
	{
		for( TMapResource::iterator it = m_mapResource.begin(), it_end = m_mapResource.end();
			it != it_end;
			it++ )
		{
			const ResourceFactoryParam& params = it->second->getFactoryParams();
			if( params.group == _resourceFile )
			{
				ResourceReference* res = it->second;
				res->decrementReference();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileUnload( const String& _resourceFile )
	{
		for( TMapResource::iterator it = m_mapResource.begin(), it_end = m_mapResource.end();
			it != it_end;
			it++ )
		{
			const ResourceFactoryParam& params = it->second->getFactoryParams();
			if( params.group == _resourceFile )
			{
				ResourceReference* res = it->second;
				while( res->decrementReference() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	void ResourceManager::_dumpResources()
	{
		FILE* file = fopen( "ResourceDump.log", "a" );
		fprintf( file, "Dumping resources...\n");
		for( TMapResource::iterator it = m_mapResource.begin()
			, it_end = m_mapResource.end()
			; it != it_end
			; it++ )
		{
			if( it->second->countReference() )
			{
				fprintf( file, "--> %s : %d\n", it->first.c_str(), it->second->countReference() );
			}
		}
		fclose( file );
	}
#endif
	//////////////////////////////////////////////////////////////////////////
}