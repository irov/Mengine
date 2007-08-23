#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

#	include "ScriptEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createEntity(
			std::string _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			PyObject * module = Holder<ScriptEngine>::hostage()
				->getEntityModule( _type );
			PyObject * result = pybind::call_method( module, _type.c_str(), "()" );

			if( result == 0 )
			{
				pybind::ret_none();
			}

			Entity * en = pybind::extract<Entity*>( result );

			if( en == 0 )
			{
				printf("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			en->setPosition( _pos );
			en->setDirection( _dir );

			return result;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		pybind::proxy_<Entity, pybind::bases<SceneNode2D> >("Entity");
			
		pybind::def( "createEntity", &ScriptMethod::createEntity );
	}
}