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

		bool exist = false;
		if( LoaderEngine::get()
			->load( m_desc.name, m_desc.description, this, exist ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file '%s %s' '%s'"
				, m_desc.path.c_str()
				, m_desc.name.c_str()
				, m_desc.description.c_str()
				);

			return false;
		}

		ScriptEngine::TListModulePath listModulePath;

		String scriptPakPath = m_baseDir + "/" + Helper::to_str(m_desc.name);

		for( TVectorString::iterator
			it = m_pathScripts.begin(),
			it_end = m_pathScripts.end();
		it != it_end;
		++it )
		{
			String path;
			Utils::collapsePath( scriptPakPath + "/" + *it, path );
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
				ConstString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Scenes_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderScenes_, path );
			}

			BIN_CASE_NODE( Protocol::Arrows )
			{
				ConstString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Arrows_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderArrows_, path );
			}

			BIN_CASE_NODE( Protocol::Entities )
			{
				ConstString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Entities_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderEntities_, path );
			}

			BIN_CASE_NODE( Protocol::Resources )
			{
				String path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Resources_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderResources_, path );
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
	void ResourcePak::_loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderScenes_( BinParser * _parser, const ConstString & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Scene_Name, &ResourcePak::addScene_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderArrows_( BinParser * _parser, const ConstString & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Arrow_Name, &ResourcePak::addArrow_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderEntities_( BinParser * _parser, const ConstString & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Entity_Name, &ResourcePak::addEntity_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderResources_( BinParser * _parser, const String & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Resource_Name, &ResourcePak::addResource_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScene_( const ConstString & _name, const ConstString & _path )
	{
		SceneDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		SceneManager::get()
			->registerScene( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addArrow_( const ConstString & _name, const ConstString & _path )
	{
		ArrowDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		ArrowManager::get()
			->registerArrow( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addEntity_( const ConstString & _name, const ConstString & _path )
	{
		PrototypeDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		EntityManager::get()
			->addPrototype( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name, const String & _path )
	{
		String path = _path;
		path += "/";
		path += Helper::to_str(_name);

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