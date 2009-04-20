
#	include "ResourceManager.h"

#	include "Application.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"

#	include "LogEngine.h"
#	include "ScriptEngine.h"

#	include "AlphaChannelManager.h"

#	include <ctime>
#	include "pybind/include/pybind/pybind.hpp"
#	include "RenderEngine.h"
#	include "Game.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
		: m_alphaChannelManager( NULL )
	{
		m_alphaChannelManager = new AlphaChannelManager();
		Holder<AlphaChannelManager>::keep( m_alphaChannelManager );
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::~ResourceManager()
	{
		//_dumpResources();
		for( TMapResource::iterator
			it = m_mapResource.begin();
			it != m_mapResource.end();
		++it)
		{
			it->second->release();
			//TFactoryResource::release( it->second->getFactoryParams().
		}

		for( TMapResource::iterator
			it = m_mapResource.begin(),
			it_end = m_mapResource.end();
		it != it_end;
		++it)
		{
			delete it->second;
			//TFactoryResource::release( it->second->getFactoryParams().
		}

		for( TMapResourceManagerListenerScript::iterator
			it = m_scriptListeners.begin(),
			it_end = m_scriptListeners.end();
		it != it_end;
		++it )
		{
			ScriptEngine::decref( it->second );
		}
		
		m_mapResource.clear();


		Holder<AlphaChannelManager>::empty();
		if( m_alphaChannelManager != NULL )
		{
			delete m_alphaChannelManager;
			m_alphaChannelManager = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::registrationType( const String& _type, Factory::TGenFunc _func )
	{
		m_factory.registration( _type, _func );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loadResource( const String& _category, const String& _group, const String& _file )
	{
		m_currentCategory = _category;
		m_currentGroup = _group;
		m_currentFile = _file;

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentGroup );
		if( it_find == m_resourceCountMap.end() )
		{
			m_resourceCountMap.insert( std::make_pair( m_currentGroup, 0 ) );
		}

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &ResourceManager::loaderDataBlock ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse resource \"%s\""
				, _file.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loaderDataBlock( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "DataBlock" )
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
			XML_CASE_NODE( "Resource" )
			{
				String name;
				String type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
					XML_CASE_ATTRIBUTE( "Type", type );
				}

				ResourceReference * resource = createResource( name, type );

				if( resource == 0 )
				{
					MENGE_LOG_ERROR( "Don't register resource type \"%s\""
						, type.c_str() );
					continue;
				}

				bool registered = registerResource( resource );

				if( registered == true )
				{
					XML_PARSE_ELEMENT( resource, &ResourceReference::loader );
				}
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
		param.file = m_currentFile;

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentCategory );
		if( it_find != m_resourceCountMap.end() )
		{
			++it_find->second;
		}

		return m_factory.generate_t<ResourceReference>( _type, param );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::registerResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return false;
		}

		const String& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find == m_mapResource.end() )
		{
			m_mapResource.insert( std::make_pair( name, _resource ) );
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Duplicate resource name \"%s\" in group \"%s\""
			, name.c_str()
			, _resource->getFactoryParams().group.c_str() );
			MENGE_LOG_ERROR( "Duplicate entry will be deleted now" );
			delete _resource;
			return false;
		}
		return true;
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
			//MENGE_LOG( "Warning: resource named \"%s\" does not exist"
			//	, _name.c_str() );
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
				String nameAnsi = Holder<Application>::hostage()->utf8ToAnsi( _name );
				PyObject* result = Holder<ScriptEngine>::hostage()
									->askFunction( it->second, "(s)", nameAnsi.c_str() );
				bool render = false;
				if( pybind::convert::is_none( result ) == true )
				{
					MENGE_LOG_ERROR( "Error: Event must have return [True/False] value" );
				}
				else
				{
					render = pybind::convert::to_bool( result );
				} 
				static std::clock_t lastLoadTime = std::clock();
				std::clock_t loadTime = std::clock();
				if( render == true && (loadTime-lastLoadTime) * CLOCKS_PER_SEC * 0.001f > 50 )
				{
					// render one frame
					Holder<RenderEngine>::hostage()->beginScene();
					Holder<Game>::hostage()->render();
					Holder<RenderEngine>::hostage()->endScene();
					Holder<RenderEngine>::hostage()->swapBuffers();
					lastLoadTime = loadTime;
				}
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

		if( it_find != m_mapResource.end() && it_find->second != NULL )
		{
			//unsigned int ref_debug = _resource->countReference();
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

			//Holder<ProfilerEngine>::hostage()->removeResourceToProfile(name);
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
			MENGE_LOG_ERROR( "Invalid parse external node \"%s\""
				, _xml.c_str() );
			return 0;
		}

		if( resource == 0 )
		{
			MENGE_LOG_ERROR( "This xml file \"%s\" has invalid external node format"
				, _xml.c_str() );
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
	void ResourceManager::_dumpResources( const std::string & _category )
	{
		FILE* file = fopen( "ResourceDump.log", "a" );
		fprintf( file, "Dumping resources... ");
		fprintf( file, _category.c_str() );
		fprintf( file, "\n" );
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
	void ResourceManager::visitResources(ResourceVisitor * _visitor, const String & _file)
	{
		for( TMapResource::iterator it = m_mapResource.begin()
			, it_end = m_mapResource.end()
			; it != it_end
			; it++ )
		{
			ResourceReference * resource = it->second;
			const ResourceFactoryParam & params = resource->getFactoryParams();
			
			if(params.file == _file)
			{
				resource->accept(_visitor);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceManager::getResourceCount( const String& _resourceFile )
	{
		TResourceCountMap::iterator it_find = m_resourceCountMap.find( _resourceFile );
		if( it_find != m_resourceCountMap.end() )
		{
			return it_find->second;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
}
