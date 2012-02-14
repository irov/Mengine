#	include "SceneManager.h"
#	include "Scene.h"

#	include <memory>

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "NodeManager.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneManager::SceneManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SceneManager::~SceneManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneManager::registerScene( const ConstString & _name, const ResourceDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneManager::hasScene( const ConstString & _name )
	{
		TMapDescriptionScenes::iterator it_find = m_descriptions.find( _name );

		if( it_find == m_descriptions.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class SceneLoadable
			: public Loadable
		{
		public:
			SceneLoadable( Scene * _scene )
				: m_scene(_scene)
			{

			}

		protected:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE_PARSE( Protocol::Scene, m_scene );
				}
			}

		protected:
			void _loaded() override
			{
				m_scene->loaded();
			}

		protected:
			Scene * m_scene;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * SceneManager::createScene( const ConstString & _name )
	{
		TMapDescriptionScenes::iterator it_find = m_descriptions.find( _name );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "SceneManager Error: Scene '%s' declaration not found"
				, _name.c_str() 
				);

			return 0;
		}

		const ResourceDesc & desc = it_find->second;

		Scene * scene = 0;

		if( desc.script == true )
		{
			scene = ScriptEngine::get()
				->createEntityT<Scene>( _name, Consts::get()->c_Scene, Consts::get()->c_Scene, _name, desc.pak, desc.path );
		}
		else
		{
			scene = NodeManager::get()
				->createNodeT<Scene>( _name, Consts::get()->c_Scene, Consts::get()->c_Scene );
		}

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "SceneManager Can't create scene '%s'"
				, _name.c_str() 
				);

			return 0;
		}

		scene->setName( _name );

		String xml_path = desc.path;
		xml_path += "/";
		xml_path += Helper::to_str(_name);
		xml_path += "/";
		xml_path += Helper::to_str(_name);

		SceneLoadable loadable(scene);

		bool exist = false;
		if( LoaderEngine::get()
			->cache( desc.pak, xml_path, &loadable, exist ) == false )
		{
			if( exist == true )
			{
				MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for scene '%s'(invalid binary)"
					, xml_path.c_str()
					, _name.c_str() 
					);

				scene->destroy();

				return 0;
			}
		}

		return scene;
	}
}