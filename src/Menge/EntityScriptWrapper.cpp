#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

#	include "Holder.h"

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

			en->setPosition( _pos );
			en->setDirection( _dir );

			PyObject * pyEn = en->getScriptable()->ptr();

			return pyEn;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		boost::python::class_<Entity, boost::python::bases<SceneNode2D> >("Entity")
			.def( boost::python::init<>("constructor") )
			;

		boost::python::def( "createEntity", &ScriptMethod::createEntity );
	}
}