#	include "ResourceManager.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"

#	include "LogEngine.h"
#	include "ScriptEngine.h"

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
	void ResourceManager::loadResource( const std::string & _file )
	{
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &ResourceManager::loaderDataBlock ) == false )
		{
			MENGE_LOG("Error: Invalid parse resource '%s' \n", _file.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loaderDataBlock( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("DataBlock")
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
			XML_CASE_NODE("Resource")
			{
				std::string name;
				std::string type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE("Name", name );
					XML_CASE_ATTRIBUTE("Type", type );
				}

				ResourceReference * resource = createResource( name, type );

				if( resource == 0 )
				{
					MENGE_LOG( "Don't register resource type '%s'\n", type.c_str() );
					continue;
				}

				registerResource( resource );

				XML_PARSE_ELEMENT( resource, &ResourceReference::loader );
			}		

			//XML_CHECK_NODE("Extend")
			//{
			//	XML_DEF_ATTRIBUTES_NODE( File );

			//	loadResource( File );
			//}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResource( const std::string & _name, const std::string & _type )
	{
		ResourceFactoryParam param;
		param.name = _name;
		return TFactoryResource::generate( _type, param );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::registerResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const std::string & name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find == m_mapResource.end() )
		{
			m_mapResource.insert( std::make_pair( name, _resource ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResource( const std::string & _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return 0;
		}

		ResourceReference * resource = it_find->second;

		size_t inc = resource->incrementReference();
		// resource has been loaded
		if( inc == 1 && m_listeners.size() )
		{
			for( TListResourceManagerListener::iterator it = m_listeners.begin();
				it != m_listeners.end();
				it++)
			{
				(*it)->onResourceLoaded();
			}

			for( TMapResourceManagerListenerScript::iterator it = m_scriptListeners.begin();
				it != m_scriptListeners.end();
				it++)
			{
				PyObject * result = 
					Holder<ScriptEngine>::hostage()
					->callFunction( it->second, "()", it->first );
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

		const std::string & name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find != m_mapResource.end() )
		{
			size_t inc = _resource->decrementReference();
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const std::string & _name )
	{
		ResourceReference * resource = getResource( _name );
		
		return resource != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const std::string & _name )
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
			->hasModuleFunction( _listener, "onHandleResourceLoaded" ) == false )
		{
			return;
		}

		PyObject * event = Holder<ScriptEngine>::hostage()
			->getModuleFunction( _listener, "onHandleResourceLoaded" );

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
	ResourceReference* ResourceManager::createResourceFromXml( const std::string& _xml )
	{
		ResourceReference* resource = 0;

		XmlResourceLoaderListener * resourceLoader = new XmlResourceLoaderListener( &resource, this );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlBuffer( _xml, resourceLoader ) == false )
		{
			MENGE_LOG("Invalid parse external node `%s`\n", _xml.c_str() );

			return 0;
		}

		if( resource == 0 )
		{
			MENGE_LOG("This xml file `%s` have invalid external node format\n", _xml.c_str() );
		}

		return resource;
	}
}
