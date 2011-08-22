#	include "ResourcePak.h"

#	include "ScriptEngine.h"
#	include "TextManager.h"
#	include "ResourceManager.h"
#	include "ParamManager.h"

#	include "SceneManager.h"
#	include "ArrowManager.h"
#	include "EntityManager.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "LogEngine.h"
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
	void ResourcePak::apply()
	{
		for( TMapSceneDesc::iterator
			it = m_scenesDesc.begin(),
			it_end = m_scenesDesc.end();
		it != it_end;
		++it )
		{
			SceneManager::get()
				->registerScene( it->first, it->second );
		}


		for( TMapArrowDesc::iterator
			it = m_arrowsDesc.begin(),
			it_end = m_arrowsDesc.end();
		it != it_end;
		++it )
		{
			ArrowManager::get()
				->registerArrow( it->first, it->second );
		}

		for( TMapPrototypeDesc::iterator
			it = m_prototypesDesc.begin(),
			it_end = m_prototypesDesc.end();
		it != it_end;
		++it )
		{
			EntityManager::get()
				->addPrototype( it->first, it->second );
		}

		for( TMapResourceDesc::iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
			ResourceManager::get()
				->loadResource( it->first, it->second );
		}

		for( TMapParamDesc::iterator
			it = m_paramsDesc.begin(),
			it_end = m_paramsDesc.end();
		it != it_end;
		++it )
		{
			ParamManager::get()
				->registerParam( it->first, it->second );
		}

		for( TMapTextDesc::iterator
			it = m_textsDesc.begin(),
			it_end = m_textsDesc.end();
		it != it_end;
		++it )
		{
			TextManager::get()
				->loadResource( it->first, it->second );
		}
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
				String path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Scenes_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderScenes_, path );
			}

			BIN_CASE_NODE( Protocol::Arrows )
			{
				String path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Arrows_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderArrows_, path );
			}

			BIN_CASE_NODE( Protocol::Entities )
			{
				String path;

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

			BIN_CASE_NODE( Protocol::Params )
			{
				String path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Params_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderParams_, path );
			}

			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scripts_Path, &ResourcePak::addScriptPath_ );

			BIN_CASE_NODE( Protocol::Texts )
			{
				String path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Texts_Path, path );
				}

				BIN_PARSE_METHOD_CARG1( this, &ResourcePak::loaderTexts_, path );
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::_loaded()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderScenes_( BinParser * _parser, const String & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Scene_Name, &ResourcePak::addScene_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderArrows_( BinParser * _parser, const String & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Arrow_Name, &ResourcePak::addArrow_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderEntities_( BinParser * _parser, const String & _path )
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
	void ResourcePak::loaderParams_( BinParser * _parser, const String & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD_CARG1( Protocol::Param_Name, &ResourcePak::addParam_, _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::loaderTexts_( BinParser * _parser, const String & _path )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Text )
			{
				ConstString Name;
				String File;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Text_Name, Name);
					BIN_CASE_ATTRIBUTE( Protocol::Text_File, File);
				}
				
				this->addText_( Name, _path, File );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScene_( const ConstString & _name, const String & _path )
	{
		ResourceDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		m_scenesDesc.insert( std::make_pair(_name, desc) );

		//SceneManager::get()
		//	->registerScene( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addArrow_( const ConstString & _name, const String & _path )
	{
		ResourceDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		m_arrowsDesc.insert( std::make_pair(_name, desc) );

		//ArrowManager::get()
		//	->registerArrow( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addEntity_( const ConstString & _name, const String & _path )
	{
		ResourceDesc desc;
		desc.pak = m_desc.name;
		desc.path = _path;

		m_prototypesDesc.insert( std::make_pair(_name, desc) );
		//EntityManager::get()
		//	->addPrototype( _name, desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const ConstString & _name, const String & _path )
	{
		ResourceDesc desc;		
		desc.pak = m_desc.name;
		desc.path = _path;

		m_resourcesDesc.insert( std::make_pair(_name, desc) );

		//ResourceManager::get()
		//	->loadResource( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addParam_( const ConstString & _name, const String & _path )
	{
		ResourceDesc desc;

		desc.pak = m_desc.name;
		desc.path = _path;

		m_paramsDesc.insert( std::make_pair(_name, desc) );

		//ParamManager::get()
		//	->registerParam( m_desc.name, _name, path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addText_( const ConstString & _name, const String & _path, const String & _file )
	{
		ResourceDesc desc;

		desc.pak = m_desc.name;
		desc.path = _path;

		m_textsDesc.insert( std::make_pair(_name, desc) );

		//TextManager::get()
		//	->addResourceFile( _name, m_desc.name, filename );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScriptPath_( const String & _name )
	{
		m_pathScripts.push_back( _name );
	}
}