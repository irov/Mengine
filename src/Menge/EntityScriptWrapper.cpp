#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"


#	include "Entity.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"

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
				printf("Cant't create entity [%s] (not module)\n"
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
				printf("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			entity->setType("Entity");
			entity->setScript( result );

			std::string xml_path = Holder<Game>::hostage()
				->getPathEntities();
			
			xml_path += '/';
			xml_path += _type;
			xml_path += ".xml";

			TiXmlDocument * document = Holder<FileEngine>::hostage()
				->loadXml( xml_path );

			if( document )
			{
				XML_FOR_EACH_DOCUMENT( document )
				{
					XML_CHECK_NODE("Entity")
					{
						entity->loader(XML_CURRENT_NODE);

						entity->registerEventMethod( "LOADER", "onLoader" );
						entity->callEvent("LOADER", "()");
					}
				}
				XML_INVALID_PARSE()
				{
					printf("Error: Invalid parse %s.xml\n", _type.c_str() );
				}
			}
			else
			{
				printf( "Warrning: not find entity xml '%s'", xml_path.c_str() );
			}

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			return result;
		}
	}

	SCRIPT_CLASS_WRAPPING( Entity );

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		pybind::proxy_<Entity, pybind::bases<SceneNode2D> >("Entity", false)
			.def( "moveTo", &Entity::moveTo )
			.def( "moveStop", &Entity::moveStop )
			;

		pybind::def( "createEntity", &ScriptMethod::createEntity );
	}
}