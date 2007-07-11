#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

#	include "ScriptEngine.h"
#	include "ScriptObject.h"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createEntity(
			const std::string & _type, 
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

				Py_RETURN_NONE;
			}

			en->setPosition( _pos );
			en->setDirection( _dir );

			PyObject * pyEn = en->getScriptable();

			return pyEn;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		boost::python::class_<Entity, boost::python::bases<SceneNode2D> > pyEntityClass("Entity");
			
		pyEntityClass.def( boost::python::init<>("constructor") );

		boost::python::def( "createEntity", &ScriptMethod::createEntity );
	}
}