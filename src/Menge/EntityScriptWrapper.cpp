#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"

#	include "ScriptDeclarationDefine.h"


#	include "Entity.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include "Game.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createEntity(
			String _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = Holder<ScriptEngine>::hostage()
				->createEntity( _type );

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			PyObject * pyEntity = entity->getEmbedding();

			return pyEntity;
		}

		static PyObject * createEntityFromXml( const String& _type, const String& _xml, const mt::vec2f & _pos, const mt::vec2f & _dir )
		{
			Entity * entity = Holder<ScriptEngine>::hostage()
				->createEntityWithXml( _type, _xml );

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			PyObject * pyEntity = entity->getEmbedding();

			return pyEntity;
		}
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Entity );
	}

	//REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	void ScriptWrapper::entityWrap()
	{
		classWrapping();

		pybind::proxy_<Entity, pybind::bases<RigidBody2D> >("Entity", false)
			.def( "moveTo", &Entity::moveTo )
			.def( "moveStop", &Entity::moveStop )
			.def( "rotateTo", &Entity::rotateTo )
			.def( "accelerateTo", &Entity::accelerateTo )
			.def( "directTo", &Entity::directTo )
			.def( "angleTo", &Entity::angleTo )
			.def( "rotateStop", &Entity::rotateStop )
			.def( "flip", &Entity::flip )
			//.def( "setScale", &Entity::setScale )
			//.def( "getScale", &Entity::getScale )
			.def( "getParent", &Entity::getParent )
			.def( "scaleTo", &Entity::scaleTo )
			.def( "scaleStop", &Entity::scaleStop )
			.def( "setVelocity", &Entity::setVelocity )
			.def( "getVelocity", &Entity::getVelocity )
			.def( "getVelocitySpeed", &Entity::getVelocitySpeed )
			.def( "enableStabilization", &Entity::enableStabilization )
			;

		pybind::def( "createEntity", &ScriptMethod::createEntity );
		pybind::def( "createEntityFromXml", &ScriptMethod::createEntityFromXml );
	}
}