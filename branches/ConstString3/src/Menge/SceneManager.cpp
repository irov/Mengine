#	include "SceneManager.h"
#	include "Scene.h"

#	include <memory>

#	include "ScriptEngine.h"

#	include "NodeManager.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SceneManager::SceneManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneManager::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneManager::finalize()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	Scene * SceneManager::createScene( const ConstString & _name )
	{
        TMapScenes::iterator it_found = m_scenes.find( _name );

        if( it_found == m_scenes.end() )
        {            
            Scene * scene = 0;

            PyObject * py_prototype = this->getPrototype( _name );

            if( py_prototype == 0 )
            {
                scene = NodeManager::get()
                    ->createNodeT<Scene>( Consts::get()->c_Scene );
            }
            else
            {
                scene = ScriptEngine::get()
                    ->createEntityT<Scene>( Consts::get()->c_Scene, py_prototype );
            }

            if( scene == 0 )
            {
                MENGE_LOG_ERROR( "SceneManager::createScene Can't create scene '%s'"
                    , _name.c_str() 
                    );

                return 0;
            }

            scene->setName( _name );

            it_found = m_scenes.insert( std::make_pair(_name, scene) ).first;
        }

        Scene * scene = it_found->second;

		return scene;
	}
}