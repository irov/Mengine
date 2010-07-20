#	include "ScriptWrapper.h"

#	include "ScriptClassWrapper.h"

#	include "Entity.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"
#	include "EntityManager.h"
#	include "Logger/Logger.h"

#	include "Scene.h"
#	include "Game.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		static PyObject * setupEntity_( 
			Entity * _entity, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			if( _entity == 0 )
			{
				return pybind::ret_none();
			}

			_entity->setLocalPosition( _pos );
			_entity->setLocalDirection( _dir );

			Game::get()
				->addHomeless( _entity );

			PyObject * embedding = _entity->getEmbedding();

			return embedding;
		}

		static PyObject * createEntity(
			const ConstString & _type, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = EntityManager::get()
								->createEntity( _type );

			return setupEntity_( entity, _pos, _dir );
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
	}
}
