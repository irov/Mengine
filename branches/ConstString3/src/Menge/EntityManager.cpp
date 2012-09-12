#	include "EntityManager.h"
#	include "Entity.h"

#	include <memory>

#	include "ScriptEngine.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _prototype )
	{		
		PyObject * py_prototype = this->getPrototype( _prototype );

		if( py_prototype == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager.createEntity: entity prototype '%s' not found"
				, _prototype.c_str()
				);

			return 0;
		}

		Entity * entity = ScriptEngine::get()
			->createEntityT<Entity>( Consts::get()->c_Entity, py_prototype );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager.createEntity: Can't create entity '%s'"
				, _prototype.c_str()
				);

			return 0; 
		}

		entity->setPrototype( _prototype );

		return entity;
	}
}