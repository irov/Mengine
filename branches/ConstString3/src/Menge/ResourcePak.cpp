#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"

#	include "SceneManager.h"
#	include "ArrowManager.h"
#	include "EntityManager.h"

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
		if( FileEngine::get()->mountFileSystem( m_desc.name, m_desc.path, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount pak '%s': '%s'"
				, m_desc.name.c_str()
				, m_desc.path.c_str()
				);

			return;
		}

		if( XmlEngine::get()
			->parseXmlFileM( m_desc.name, m_desc.description, this, &ResourcePak::loaderPak_ ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file '%s'"
				, m_desc.description.c_str() 
				);
		}

		ScriptEngine::TListModulePath listModulePath;

		if( m_pathScripts.empty() == false )
		{
			listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathScripts );
		}

		if( m_pathEntities.empty() == false )
		{
			listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathEntities );
		}

		if( m_pathScenes.empty() == false )
		{
			listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathScenes );
		}

		if( m_pathArrows.empty() == false )
		{
			listModulePath.push_back( m_baseDir + "/" + m_desc.name.str() + "/" + m_pathArrows );
		}

		ScriptEngine::get()
			->addModulePath( listModulePath );
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

			XML_CASE_ATTRIBUTE_NODE_METHOD( "Text", "Path", &ResourcePak::setTextsPath_ );
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

		SceneManager::get()
			->registerScene( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addArrow_( const ConstString & _name )
	{
		ArrowDesc desc;
		desc.pak = m_desc.name;
		desc.path = m_pathArrows;

		ArrowManager::get()
			->registerArrow( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addEntity_( const ConstString & _name )
	{
		EntityDesc desc;
		desc.pak = m_desc.name;
		desc.path = m_pathEntities;

		EntityManager::get()
			->registerEntityType( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name )
	{
		String path = m_pathResources;
		path += "/";
		path += _name.str();
		path += ".resource";

		ResourceManager::get()
			->loadResource( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::setTextsPath_( const String & _path )
	{
		TextManager::get()
			->loadResourceFile( m_desc.name, _path );
	}
}