
#	include "ResourceManager.h"

#	include "Application.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"

#	include "LogEngine.h"
#	include "ScriptEngine.h"

#	include <ctime>
#	include "pybind/include/pybind/pybind.hpp"
#	include "RenderEngine.h"
#	include "Game.h"

#	include "ResourceAnimation.h"
#	include "ResourceCapsuleController.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceFont.h"
#	include "ResourceTilePolygon.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceImageCell.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageSet.h"
#	include "ResourceVideo.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ResourcePhysicBoxGeometry.h"
#	include "ResourcePhysicConcaveGeometry.h"
#	include "ResourcePhysicConvexGeometry.h"
#	include "ResourcePlaylist.h"
#	include "ResourceSound.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "ResourceMeshMS3D.h"
#	include "ResourceMeshNoise.h"
#	include "ResourceMaterial.h"
#	include "ResourceWindow.h"
#	include "ResourceHotspotImage.h"

#	include "Utils.h"

#	define RESOURCE_FACTORY( Type )\
	this->registerFactory( #Type , Type::genFactory() )

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
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loadResource( const String& _category, const String& _group, const String& _file )
	{

		TResourcePackMap::iterator it_find_pack = m_resourcePackMap.find( _group );
		if( it_find_pack != m_resourcePackMap.end() )
		{
			MENGE_LOG_ERROR( "Warning: Resource group \"%s\" already exist",
				_group.c_str() );
		}
		else
		{
			m_resourcePackMap.insert( std::make_pair( _group, _category ) );
		}

		m_currentCategory = _category;
		m_currentGroup = _group;
		m_currentFile = _file;

		TResourceCountMap::iterator it_find = m_resourceCountMap.find( m_currentGroup );
		if( it_find == m_resourceCountMap.end() )
		{
			m_resourceCountMap.insert( std::make_pair( m_currentGroup, 0 ) );
		}

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _category, _file, this, &ResourceManager::loaderDataBlock ) == false )
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

		const String& name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find == m_mapResource.end() )
		{
			m_mapResource.insert( std::make_pair( name, _resource ) );
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Duplicate resource name '%s' in group '%s'"
				, name.c_str()
				, _resource->getFactoryParams().group.c_str()
				);

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
				Holder<ScriptEngine>::hostage()
					->callFunction( it->second, "(s)", nameAnsi.c_str() );
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
			MENGE_LOG_ERROR( "This xml file '%s' has invalid external node format"
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
			if( it->second->countReference() )
			{
				fprintf( file, "--> %s : %d\n", it->first.c_str(), it->second->countReference() );
			}
		}
		fclose( file );
	}
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
			
			if(params.group == _file)
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
		MENGE_LOG_ERROR( "Warning: (ResourceManager::getResourceCount) Resource File '%s' not declared"
			, _resourceFile.c_str() );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::initialize()
	{
		MENGE_LOG( "Creating Resource Factory..." );
		RESOURCE_FACTORY( ResourceAnimation );
		//RESOURCE_FACTORY( ResourceCapsuleController );
		RESOURCE_FACTORY( ResourceEmitterContainer );
		RESOURCE_FACTORY( ResourceFont );
		RESOURCE_FACTORY( ResourceImageAtlas );
		RESOURCE_FACTORY( ResourceTilePolygon );
		RESOURCE_FACTORY( ResourceImageCell );
		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageDynamic );
		RESOURCE_FACTORY( ResourceImageSet );
		RESOURCE_FACTORY( ResourceVideo );
		RESOURCE_FACTORY( ResourceMesh );
		//RESOURCE_FACTORY( ResourceSkeleton );
		//RESOURCE_FACTORY( ResourcePhysicBoxGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConcaveGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConvexGeometry );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceTileMap );
		RESOURCE_FACTORY( ResourceTileSet );
		RESOURCE_FACTORY( ResourceMeshMS3D );
		RESOURCE_FACTORY( ResourceMeshNoise );
		//RESOURCE_FACTORY( ResourceMaterial );
		RESOURCE_FACTORY( ResourceWindow );
		RESOURCE_FACTORY( ResourceHotspotImage );
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
