#	include "SceneManager.h"
#	include "Scene.h"

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

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
		for( TMapScenes::iterator
			it = m_scenes.begin(),
			it_end = m_scenes.end();
		it != it_end;
		++it)
		{
			Scene * scene = it->second;

			if( scene->isSubScene() == false )
			{
				scene->destroy();
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneManager::registerScene( const ConstString & _name, const SceneDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * SceneManager::getScene( const ConstString & _name )
	{
		TMapScenes::iterator it_find = m_scenes.find( _name );

		if( it_find == m_scenes.end() )
		{
			Scene * scene = this->createScene_( _name );

			if( scene == 0 )
			{
				return 0;
			}

			it_find = m_scenes.insert( std::make_pair( _name, scene ) ).first;
		}

		it_find->second->incrementReference();

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneManager::destroyScene( const ConstString & _name )
	{
		TMapScenes::iterator it_find = m_scenes.find( _name );

		if( it_find == m_scenes.end() )
		{
			MENGE_LOG_ERROR( "Can't find scene '%s' to remove"
				, _name.c_str() 
				); 

			return false;
		}
		Scene * scene = it_find->second;

		if( scene->decrementReference() != 0 )
		{
			return false;
		}

		it_find->second->destroy();

		m_scenes.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * SceneManager::createScene_( const ConstString & _name )
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
			->createNodeT<Scene>( _name, Consts::get()->c_Scene, Consts::get()->c_Scene );

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "Can't create scene '%s'"
				, _name.c_str() 
				); 

			return 0;
		}

		scene->setName( _name );

		String xml_path = desc.path;
		xml_path += "/";
		xml_path += _name.str();
		xml_path += "/Scene.xml";

		if( LoaderEngine::get()
			->load( desc.pak, xml_path, scene ) == false )
		//if( XmlEngine::get()
		//	->parseXmlFileM( desc.pak, xml_path, scene, &Scene::loader ) == false )
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