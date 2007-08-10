#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

#	include "ScriptEngine.h"
#	include "ScriptObject.h"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createEntity(
			std::string _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity *en = Holder<ScriptEngine>::hostage()
				->createEntity( _type );

			if( en == 0 )
			{
				printf("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				pybind::ret_none();
			}

			en->setPosition( _pos );
			en->setDirection( _dir );

			PyObject * pyEn = en->getScriptable();

			return pyEn;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		pybind::class_<Entity, pybind::bases<SceneNode2D> >("Entity");
			
		pybind::def( "createEntity", &ScriptMethod::createEntity );
	}
}