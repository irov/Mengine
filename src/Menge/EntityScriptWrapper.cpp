#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

#	include "FileEngine.h"
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

			Entity * entity = pybind::extract<Entity*>( result );

			if( entity == 0 )
			{
				printf("Can't create entity [%s]\n"
					, _type.c_str() 
					);

				return pybind::ret_none();
			}

			std::string xml_path = Holder<ScriptEngine>::hostage()
				->getEntitiesPath();
			
			xml_path += '/';
			xml_path += _type;
			xml_path += ".xml";

			TiXmlDocument * document = Holder<FileEngine>::hostage()
				->loadXml( xml_path );

			XML_FOR_EACH_DOCUMENT( document )
			{
				XML_CHECK_NODE("Entity")
				{
					entity->loader(XML_CURRENT_NODE);
				}
			}
			XML_INVALID_PARSE()
			{
				
			}

			entity->setPosition( _pos );
			entity->setDirection( _dir );

			return result;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	{
		pybind::proxy_<Entity, pybind::bases<SceneNode2D> >("Entity");
			
		pybind::def( "createEntity", &ScriptMethod::createEntity );
	}
}