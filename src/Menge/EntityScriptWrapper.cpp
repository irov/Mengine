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
			std::string _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
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

			Entity * entity = pybind::extract<Entity*>( result );

			if( entity == 0 )
			{
				MENGE_LOG("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			entity->setType("Entity");
			entity->setScript( result );

			std::string xml_path = Holder<Game>::hostage()
				->getPathEntities( _type );
			
			xml_path += '/';
			xml_path += _type;
			xml_path += ".xml";

			TVectorChar * entityXml = 
				Holder<ScriptEngine>::hostage()
				->getEntityXML( _type );

			if( entityXml )
			{
				if( Holder<XmlEngine>::hostage()
					->parseXmlBufferM( *entityXml, entity, &Entity::loader ) )
				{
					entity->registerEvent( "LOADER", "onLoader" );
					entity->callEvent("LOADER", "()");
				}
			}

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			return result;
		}

		static PyObject * createEntityFromXml( const std::string& _type, const std::string & _xml, const mt::vec2f & _pos, const mt::vec2f & _dir )
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

			Entity * entity = pybind::extract<Entity*>( result );

			if( entity == 0 )
			{
				MENGE_LOG("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			entity->setType("Entity");
			entity->setScript( result );

			std::string xml_path = Holder<Game>::hostage()
				->getPathEntities( _type );

			xml_path += '/';
			xml_path += _type;
			xml_path += ".xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlBufferM( _xml, entity, &Entity::loader ) )
			{
				entity->registerEvent( "LOADER", "onLoader" );
				entity->callEvent("LOADER", "()");
			}

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			return result;
		}
	}

	SCRIPT_CLASS_WRAPPING( Entity );

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		pybind::proxy_<Entity, pybind::bases<RigidBody2D> >("Entity", false)
			.def( "moveTo", &Entity::moveTo )
			.def( "moveStop", &Entity::moveStop )
			.def( "rotateTo", &Entity::rotateTo )
			.def( "accelerateTo", &Entity::accelerateTo )
			.def( "rotateStop", &Entity::rotateStop )
			.def( "flip", &Entity::flip )
			.def( "setScale", &Entity::setScale )
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