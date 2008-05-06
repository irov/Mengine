#	include "ScriptWrapper.h"

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
				->getPathEntities( _type );
			
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

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Actor );
	}

	//REGISTER_SCRIPT_CLASS( Menge, Actor, Node )
	void ScriptWrapper::actorWrap()
	{
		classWrapping();
		
		pybind::proxy_<Actor, pybind::bases<SceneNode3D> >("Actor", false)
			.def( "setRigidBody", &Actor::setRigidBody )
			.def( "setAnimationEnabled", &Actor::setAnimationEnabled )
			.def( "playAnimation", &Actor::playAnimation )
			.def( "pauseAnimation", &Actor::pauseAnimation )
			.def( "stopAnimation", &Actor::stopAnimation )
			.def( "stopAllAnimations", &Actor::stopAllAnimations )
			.def( "setMoveSpeed", &Actor::setMoveSpeed )
			.def( "setYawSpeed", &Actor::setYawSpeed )
			.def( "setPitchSpeed", &Actor::setPitchSpeed )
			.def( "setRollSpeed", &Actor::setRollSpeed )
			.def( "attachEntity", &Actor::attachEntity )
			//.def( "setParentActor", &Actor::setParentActor )
			//.def( "setParentRoot", &Actor::setParentRoot )
			//.def( "attachEntity", &Actor::attachEntity )
		//	.def( "update", &Actor::_update )
		;

		pybind::def( "createActor", &ScriptMethod::createActor );
	}
}