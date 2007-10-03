#	include "ResourceManager.h"

#	include "ResourceDeclare.h"
#	include "ResourceImplement.h"

#	include "ResourceImageMNG.h"
#	include "ResourceImageCell.h"

#	include "FileEngine.h"

#	include "ErrorMessage.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::ResourceManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceManager::~ResourceManager()
	{
		for each( const TMapResource::value_type & it in m_mapResource )
		{
			it.second->release();
			delete it.second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceManager::loadResource( const std::string & _file )
	{
		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( _file );

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE_FOR_EACH("DataBlock")
			{
				XML_CHECK_NODE("Resource")
				{
					XML_DEF_ATTRIBUTES_NODE( Name );
					XML_DEF_ATTRIBUTES_NODE( Type );

					Resource * resource = createResource( Name, Type );

					if( resource == 0 )
					{
						ErrorMessage( "Don't register resource type '%s' ", Type.c_str() );
						continue;
					}
			
					resource->loader( XML_CURRENT_NODE );

					registerResource( resource );
				}		

				XML_CHECK_NODE("Extend")
				{
					XML_DEF_ATTRIBUTES_NODE( File );

					loadResource( File );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Resource * ResourceManager::createResource( const std::string & _name, const std::string & _type )
	{
		ResourceFactoryParam param;
		param.name = _name;
		return TFactoryResource::generate( _type, param );
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

				delete _resource;

				m_mapResource.erase( it_find );
			}
		}
	}
}