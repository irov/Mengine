
#	include "ResourceManager.h"

#	include "Application.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"

#	include "Factory/FactoryIdentity.h"

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
	ResourceManager::ResourceManager( FactoryIdentity * _factoryIdentity )
		: m_factoryIdentity(_factoryIdentity)
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
	bool ResourceManager::loadResource( const String& _category, const String& _group, const String& _file )
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
		m_currentFile = _file;

		m_resourcePackMap.insert( std::make_pair( _group, _category ) );

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentGroup );
		if( it_find == m_resourceCountMap.end() )
		{
			m_resourceCountMap.insert( std::make_pair( m_currentGroup, 0 ) );
		}

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _category, _file, this, &ResourceManager::loaderDataBlock ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse resource '%s'"
				, _file.c_str() 
				);

			return false;
		}

		return true;
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
					MENGE_LOG_ERROR( "Don't register resource type '%s'"
						, type.c_str() 
						);

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
	ResourceReference * ResourceManager::createResourceWithParam( const String& _type, const ResourceFactoryParam & _param )
	{
		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			return 0;
		}

		resource->initialize( _param, m_factoryIdentity );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::createResourceWithIdentity( const String& _type, const ResourceFactoryIdentity & _param )
	{
		ResourceReference * resource = 
			this->createObjectT<ResourceReference>( _type );

		if( resource == 0 )
		{
			return 0;
		}

		resource->initialize( _param, m_factoryIdentity );

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::registerResource( ResourceReference * _resource )
	{
		if( _resource == 0 )
		{
			return false;
		}

		//const String& name = _resource->getName();
		std::size_t nameIdentity = _resource->getNameIdentity();

		TMapResource::iterator it_find = m_mapResource.find( nameIdentity );

		if( it_find == m_mapResource.end() )
		{
			TListResource::iterator it_insert = this->cacheGroupResource_( _resource );

			m_mapResource.insert( std::make_pair( nameIdentity, it_insert ) );			
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
		std::size_t categoryIdentity = _resource->getCategoryIdentity();

		TCacheGroupResource::iterator it_find = m_cacheGroupResource.find( categoryIdentity );

		if( it_find == m_cacheGroupResource.end() )
		{
			it_find = m_cacheGroupResource.insert( std::make_pair( categoryIdentity, TListResource() ) ).first;
		}

		TListResource::iterator it_insert = it_find->second.insert( it_find->second.end(), _resource );

		return it_insert;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::unregisterResource( ResourceReference* _resource )
	{
		std::size_t nameIdentity = _resource->getNameIdentity();

		TMapResource::iterator it_resource_find = m_mapResource.find( nameIdentity );

		if( it_resource_find == m_mapResource.end() )
		{			
			MENGE_LOG_ERROR( "Warning: Unregister unknown resource name '%s' in group '%s' category '%s'"
				, _resource->getName().c_str()
				, _resource->getGroup().c_str()
				, _resource->getCategory().c_str()
				);

			return;
		}

		std::size_t categoryIdentity = _resource->getCategoryIdentity();

		TCacheGroupResource::iterator it_category_find = m_cacheGroupResource.find( categoryIdentity );

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
	ResourceReference * ResourceManager::getResourceByName( const String& _name )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _name );

		if( identity == -1 )
		{
			return 0;
		}

		return getResourceByIdentity( identity );
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceManager::getResourceByIdentity( std::size_t _identity )
	{
		TMapResource::iterator it_find = m_mapResource.find( _identity );

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
			const String & name = resource->getName();

			for( TListResourceManagerListener::iterator 
				it = m_listeners.begin(),
				it_end = m_listeners.end();
			it != it_end;
			++it)
			{
				(*it)->onResourceLoaded( name );
			}

			for( TMapResourceManagerListenerScript::iterator 
				it = m_scriptListeners.begin(),
				it_end = m_scriptListeners.end();
			it != it_end;
			++it)
			{
				String nameAnsi = Application::hostage()
					->utf8ToAnsi( name );

				ScriptEngine::hostage()
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

		//const String& name = _resource->getName();
		std::size_t nameIdentity = _resource->getNameIdentity();

		TMapResource::iterator it_find = m_mapResource.find( nameIdentity );

		if( it_find != m_mapResource.end() && it_find->second != NULL )
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
		ResourceReference * resource = getResourceByName( _name );
		
		return resource != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceRelease( const String& _name )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _name );

		if( identity == -1 )
		{
			return;
		}

		TMapResource::iterator it_find = m_mapResource.find( identity );

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
			MENGE_LOG_ERROR( "Invalid parse external node '%s'"
				, _xml.c_str() 
				);

			return 0;
		}

		if( resource == 0 )
		{
			MENGE_LOG_ERROR( "This xml file '%s' has invalid external node format"
				, _xml.c_str() 
				);
		}

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceUnload( const String& _name )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _name );

		if( identity == -1 )
		{
			return;
		}

		TMapResource::iterator it_find = m_mapResource.find( identity );

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
	void ResourceManager::directResourceFileCompile( const String& _resourceFile )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _resourceFile );

		if( identity == -1 )
		{
			return;
		}

		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( identity );

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
			std::size_t nameIdentity = (*it)->getNameIdentity();
			getResourceByIdentity( nameIdentity );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::directResourceFileRelease( const String& _resourceFile )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _resourceFile );

		if( identity == -1 )
		{
			return;
		}

		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( identity );

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
	void ResourceManager::directResourceFileUnload( const String& _resourceFile )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _resourceFile );

		if( identity == -1 )
		{
			return;
		}

		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( identity );

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
	void ResourceManager::dumpResources( const std::string & _category )
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
			ResourceReference * ref = *it->second;

			if( ref->countReference() )
			{
				const String & name = m_factoryIdentity->getIdentity( it->first );

				fprintf( file, "--> %s : %d\n"
					, name.c_str()
					, ref->countReference() 
					);
			}
		}
		fclose( file );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::visitResources( ResourceVisitor * _visitor, const String & _resourceFile )
	{
		std::size_t identity = m_factoryIdentity->findIdentity( _resourceFile );

		if( identity == -1 )
		{
			return;
		}

		TCacheGroupResource::iterator it_found = m_cacheGroupResource.find( identity );

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
	size_t ResourceManager::getResourceCount( const String& _resourceFile )
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
	const String& ResourceManager::getCategoryResource( const String& _group ) const
	{
		TResourcePackMap::const_iterator it_find = m_resourcePackMap.find( _group );
		if( it_find != m_resourcePackMap.end() )
		{
			return it_find->second;
		}

		return Utils::emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
}
