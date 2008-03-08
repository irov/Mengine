#	include "ScriptClassWrapperDefine.h"

#	include "ScriptDeclarationDefine.h"


#	include "Actor.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include "Game.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createActor(
			std::string _name,
			std::string _type
			)
		{
			PyObject * module = Holder<ScriptEngine>::hostage()
				->getEntityModule( _type );

			if( module == 0 )
			{
				MENGE_LOG("Cant't create entity [%s] (not module)\n"
					, _type.c_str() 
					);
			}

			PyObject * result = pybind::call_method( module, _type.c_str(), "()" );

			if( result == 0 )
			{
				return pybind::ret_none();
			}

			Actor * actor = pybind::extract<Actor*>( result );

			if( actor == 0 )
			{
				MENGE_LOG("Can't create actor [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			actor->setName( _name );
			actor->setType( "Actor" );
			actor->setScript( result );

			std::string xml_path = Holder<Game>::hostage()
				->getPathEntities();
			
			actor->attachToRootNode();

			xml_path += '/';
			xml_path += _type;
			xml_path += ".xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, actor, &Actor::loader ) )
			{
				actor->registerEvent( "LOADER", "onLoader" );
				actor->callEvent("LOADER", "()");
			}

		//	actor->setLocalPosition( _pos );
		//	actor->setLocalOrient( _dir );

			return result;
		}
	}

	SCRIPT_CLASS_WRAPPING( Actor );

	REGISTER_SCRIPT_CLASS( Menge, Actor, Node )
	{
		pybind::proxy_<Actor, pybind::bases<SceneNode3D> >("Actor", false)
			.def( "step", &Actor::step )
			.def( "stop", &Actor::stop )
			.def( "left", &Actor::left )
			.def( "setController", &Actor::setController )
			.def( "setRigidBody", &Actor::setRigidBody )
			//.def( "setParentActor", &Actor::setParentActor )
			//.def( "setParentRoot", &Actor::setParentRoot )
			//.def( "attachEntity", &Actor::attachEntity )
			//.def( "update", &Actor::update )
		;

		pybind::def( "createActor", &ScriptMethod::createActor );
	}
}