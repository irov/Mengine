#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"

#	include "ScriptDeclarationDefine.h"


#	include "Entity.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include "Scene.h"
#	include "Game.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		PyObject * createEntity(
			const String & _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = Holder<ScriptEngine>::hostage()
				->createEntity( _type );

			if( entity == 0 )
			{
				return pybind::ret_none();
			}

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			PyObject * embedding = entity->getEmbedding();

			Holder<Game>::hostage()
				->addHomeless( embedding );

			return embedding;
		}

		static PyObject * createEntityFromXml( 
			const String& _type, 
			const String& _xml, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = Holder<ScriptEngine>::hostage()
				->createEntityWithXml( _type, _xml );

			if( entity == 0 )
			{
				return pybind::ret_none();
			}

			entity->setLocalPosition( _pos );
			entity->setLocalDirection( _dir );

			PyObject * embedding = entity->getEmbedding();

			Holder<Game>::hostage()
				->addHomeless( embedding );

			return embedding;
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

		pybind::proxy_<Entity, pybind::bases<Node> >("Entity", false)
			;

		pybind::def( "createEntity", &ScriptMethod::createEntity );
		pybind::def( "createEntityFromXml", &ScriptMethod::createEntityFromXml );
	}
}
