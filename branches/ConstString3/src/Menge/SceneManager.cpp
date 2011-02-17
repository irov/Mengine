#	include "SceneManager.h"
#	include "Scene.h"

#	include <memory>

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

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
	void SceneManager::registerScene( const ConstString & _name, const SceneDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
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
			MENGE_LOG_ERROR( "Error: Scene '%s' declaration not found"
				, _name.c_str() 
				);

			return 0;
		}

		const SceneDesc & desc = it_find->second;

		Scene * scene = ScriptEngine::get()
			->createEntityT<Scene>( _name, Consts::get()->c_Scene, desc.pak, desc.path );

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "Can't create scene '%s'"
				, _name.c_str() 
				); 

			return 0;
		}

		scene->setName( _name );

		String xml_path = Helper::to_str(desc.path);
		xml_path += "/";
		xml_path += Helper::to_str(_name);
		xml_path += "/";
		xml_path += Helper::to_str(_name);

		std::auto_ptr<SceneLoadable> loadable( new SceneLoadable(scene) );

		if( LoaderEngine::get()
			->load( desc.pak, xml_path, loadable.get() ) == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for scene '%s'"
				, xml_path.c_str()
				, _name.c_str() 
				);

			return 0;
		}

		return scene;
	}
}