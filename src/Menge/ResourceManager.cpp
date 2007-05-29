#	include "ResourceManager.h"

#	include "ResourceImageMNGManager.h"

#	include "ResourceImageMNG.h"
#	include "ResourceImageCNG.h"

#	include "FileEngine.h"
#	include "XmlParser.h"
#	include "Holder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
	{
		loadResource( "Game/Resource.xml" );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loadResource( const std::string & _file )
	{
		XML_PARSE_FILE_EX( _file )
		{
			XML_CHECK_NODE_FOR_EACH("Resource")
			{
				XML_CHECK_NODE("ResourceImageMNG")
				{
					XML_DEF_ATTRIBUTES_NODE( Name );

					ResourceImageMNG * resource = new ResourceImageMNG( Name );

					resource->loader( XML_CURRENT_NODE );

					registerResource( resource );
				}		
				XML_CHECK_NODE("ResourceImageCNG")
				{
					XML_DEF_ATTRIBUTES_NODE( Name );

					ResourceImageCNG * resource = new ResourceImageCNG( Name );

					resource->loader( XML_CURRENT_NODE );

					registerResource( resource );
				}	
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::registerResource( Resource * _resource )
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
	Resource * ResourceManager::getResource( const std::string & _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return 0;
		}

		Resource * resource = it_find->second;

		if( resource->refCount() == 0 )
		{
			resource->compile();
		}

		resource->incRef();

		return resource;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::releaseResource( Resource * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const std::string & name = _resource->getName();

		TMapResource::iterator it_find = m_mapResource.find( name );

		if( it_find != m_mapResource.end() )
		{
			size_t refcounter = _resource->decRef();

			if( refcounter == 0 )
			{
				_resource->release();
			}
		}
	}
}