#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( const ResourcePakDesc & _desc, const String & _baseDir )
		: m_desc(_desc)
		, m_baseDir(_baseDir)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::preload() const
	{
		return m_desc.preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getName() const
	{
		return m_desc.name;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourcePak::getPath() const
	{
		return m_desc.path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::load()
	{
		if( FileEngine::hostage()->mountFileSystem( m_desc.name, m_desc.path, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount pak '%s': '%s'"
				, m_desc.name.c_str()
				, m_desc.path.c_str()
				);

			return;
		}

		if( XmlEngine::hostage()
			->parseXmlFileM( m_desc.name, m_desc.description, this, &ResourcePak::loaderPak_ ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file '%s'"
				, m_desc.description.c_str() 
				);
		}

		ScriptEngine::TListModulePath listModulePath;

		listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathScripts );
		listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathEntities );
		listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathScenes );
		listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathArrows );

		ScriptEngine::hostage()
			->addModulePath( listModulePath );

		for( TSetEntities::iterator
			it = m_entities.begin(),
			it_end = m_entities.end();
		it != it_end;
		++it )
		{
			ScriptEngine::hostage()
				->registerEntityType( m_desc.name, m_pathEntities, *it );
		}

		TextManager::hostage()
			->loadResourceFile( m_desc.name, m_pathTexts );

		for( TSetResources::iterator
			it = m_resources.begin(),
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			String path = m_pathResources;
			path += "/";
			path += (*it).str();
			path += ".resource";

			ConstString cpath = ConstManager::hostage()
				->genString( path );

			ResourceManager::hostage()
				->loadResource( m_desc.name, *it, cpath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderPak_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Resources" )
			{
				XML_PARSE_ELEMENT( this, &ResourcePak::loaderResources_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Scenes" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathScenes );
				}

				XML_PARSE_ELEMENT( this, &ResourcePak::loaderScenes_ );
			}

			XML_CASE_NODE( "Arrows" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathArrows );
				}

				XML_PARSE_ELEMENT( this, &ResourcePak::loaderArrows_ );
			}

			XML_CASE_NODE( "Entities" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathEntities );
				}

				XML_PARSE_ELEMENT( this, &ResourcePak::loaderEntities_ );
			}

			XML_CASE_NODE( "Resource" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathResources );
				}

				XML_PARSE_ELEMENT( this, &ResourcePak::loaderResource_ );
			}

			XML_CASE_NODE( "Scripts" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathScripts );
				}
			}

			XML_CASE_NODE( "Text" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathTexts );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderScenes_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Scene", "Name", &ResourcePak::addScene_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Arrow", "Name", &ResourcePak::addArrow_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Entity", "Name", &ResourcePak::addEntity_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderResource_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Resource", "Name", &ResourcePak::addResource_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//void ResourcePak::addModulePath_( const String & _path )
	//{
	//	m_listModulePath.push_back( m_baseDir + "/" + m_desc.name + "/" + _path );
	//}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScene_( const ConstString & _name )
	{
		SceneDesc desc;
		desc.pak = m_desc.name;
		desc.path = m_pathScenes;

		SceneManager::hostage()
			->registerScene( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addArrow_( const ConstString & _name )
	{
		m_arrows.insert( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addEntity_( const ConstString & _name )
	{
		m_entities.insert( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name )
	{
		m_resources.insert( _name );
	}
}