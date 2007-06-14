#	include "ScriptDeclarationDefine.h"

#	include "SceneManager.h"
#	include "ScriptEngine.h"
#	include "Entity.h"

namespace Menge
{

	namespace ScriptSceneManager
	{
		Entity * createEntity( const std::string & _type, const std::string & _name/*, const mt::vec2f & _pos*/ )
		{
			Entity *en = Holder<ScriptEngine>::hostage()
				->createEntity( _type, _name );

			return en;
		}

	}

	REGISTER_SCRIPT_CLASS( Menge, SceneManager )
	{
		boost::python::def( "createEntity"
			, &ScriptSceneManager::createEntity
			, boost::python::return_value_policy<boost::python::reference_existing_object>() 
			);
	}
}