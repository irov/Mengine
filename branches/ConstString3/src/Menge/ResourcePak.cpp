#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"

#	include "SceneManager.h"
#	include "ArrowManager.h"
#	include "EntityManager.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "Logger/Logger.h"
#	include "Utils/Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( const ResourcePakDesc & _desc, const String & _baseDir )
		: m_desc(_desc)
		, m_baseDir(_baseDir)
	{
	}
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
	bool ResourcePak::load()
	{
		if( FileEngine::get()
			->mountFileSystem( m_desc.name, m_desc.path, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount pak '%s': '%s'"
				, m_desc.name.c_str()
				, m_desc.path.c_str()
				);

			return false;
		}

		if( LoaderEngine::get()
			->load( m_desc.name, m_desc.description, this ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file '%s'"
				, m_desc.description.c_str() 
				);

			return false;
		}

		ScriptEngine::TListModulePath listModulePath;

		for( TVectorString::iterator
			it = m_pathScripts.begin(),
			it_end = m_pathScripts.end();
		it != it_end;
		++it )
		{
			String path;
			Utils::collapsePath( m_baseDir + "/" + m_desc.name.str() + "/" + *it, path );
			listModulePath.push_back( path );
		}

		ScriptEngine::get()
			->addModulePath( listModulePath );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::Pak, this, &ResourcePak::loaderPak_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderPak_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Scenes )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Scenes_Path, m_pathScenes );
				}

				BIN_PARSE_METHOD( this, &ResourcePak::loaderScenes_ );
			}

			BIN_CASE_NODE( Protocol::Arrows )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Arrows_Path, m_pathArrows );
				}

				BIN_PARSE_METHOD( this, &ResourcePak::loaderArrows_ );
			}

			BIN_CASE_NODE( Protocol::Entities )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Entities_Path, m_pathEntities );
				}

				BIN_PARSE_METHOD( this, &ResourcePak::loaderEntities_ );
			}

			BIN_CASE_NODE( Protocol::Resources )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Resources_Path, m_pathResources );
				}

				BIN_PARSE_METHOD( this, &ResourcePak::loaderResources_ );
			}

			BIN_CASE_NODE( Protocol::Scripts )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scripts_Path, &ResourcePak::addScriptPath_ );
				}
			}

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Text_Path, &ResourcePak::setTextsPath_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderScenes_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scene_Name, &ResourcePak::addScene_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderArrows_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Arrow_Name, &ResourcePak::addArrow_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderEntities_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Entity_Name, &ResourcePak::addEntity_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderResources_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Resource_Name, &ResourcePak::addResource_ );
		}
	}
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
			->registerEntity( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name )
	{
		String path = m_pathResources;
		path += "/";
		path += _name.str();

		ResourceManager::get()
			->loadResource( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::setTextsPath_( const String & _path )
	{
		TextManager::get()
			->loadResourceFile( m_desc.name, _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScriptPath_( const String & _name )
	{
		m_pathScripts.push_back( _name );
	}
}