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
	Scene * SceneManager::createScene( const ConstString & _name )
	{
		Scene * scene = 0;

		PyObject * py_prototype = this->getPrototype( _name );

		if( py_prototype == 0 )
		{
			scene = NodeManager::get()
				->createNodeT<Scene>( _name, Consts::get()->c_Scene, Consts::get()->c_Scene );
		}
		else
		{
			scene = ScriptEngine::get()
				->createEntityT<Scene>( _name, Consts::get()->c_Scene, Consts::get()->c_Scene, py_prototype );
		}

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "SceneManager::createScene Can't create scene '%s'"
				, _name.c_str() 
				);

			return 0;
		}

		scene->setName( _name );

		return scene;
	}
}