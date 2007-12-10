#	include "ResourceManager.h"

#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "ResourceReference.h"

#	include "FileEngine.h"

#	include "LogEngine.h"

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

		if( document == 0 )
		{
			MENGE_LOG("Error: Invalid parse resource '%s' \n", _file.c_str() );
		}

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE_FOR_EACH("DataBlock")
			{
				XML_CHECK_NODE("Resource")
				{
					XML_DEF_ATTRIBUTES_NODE( Name );
					XML_DEF_ATTRIBUTES_NODE( Type );

					ResourceReference * resource = createResource( Name, Type );

					if( resource == 0 )
					{
						MENGE_LOG( "Don't register resource type '%s'\n", Type.c_str() );
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
		XML_INVALID_PARSE()
		{
			MENGE_LOG("Error: Invalid pasrse resorce - '%s'", _file.c_str() );
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

		resource->incrementReference();

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
			_resource->decrementReference();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceManager::directResourceCompile( const std::string & _name )
	{
		TMapResource::iterator it_find = m_mapResource.find( _name );

		if( it_find == m_mapResource.end() )
		{
			return false;
		}

		ResourceReference * ref = it_find->second;
		bool result = ref->compile();
		return result;
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
		ref->release();
	}

}