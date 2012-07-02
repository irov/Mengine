#	include "EntityManager.h"
#	include "Entity.h"

#	include <memory>

#	include "ScriptEngine.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	EntityManager::EntityManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	EntityManager::~EntityManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityManager::registerPrototype( const ResourceDesc & _desc )
	{
		TMapDescriptionEntities::iterator it_found = m_descriptions.find( _desc.name );

		if( it_found != m_descriptions.end() )
		{
			MENGE_LOG_INFO("EntityManager::registerPrototype: already exist entity type %s (override)"
				, _desc.name.c_str()
				);

			m_descriptions[_desc.name] = _desc;
		}
		else
		{
			m_descriptions.insert( std::make_pair(_desc.name, _desc) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::getPrototypeDesc( const ConstString & _type, ResourceDesc & _desc )
	{
		TMapDescriptionEntities::const_iterator it_found = m_descriptions.find( _type );

		if( it_found == m_descriptions.end() )
		{
			return false;
		}

		_desc = it_found->second;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _name, const ConstString & _prototype, const ConstString & _tag )
	{
		TMapDescriptionEntities::iterator it_find = m_descriptions.find( _prototype );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "EntityManager::createEntity: Entity '%s''%s' declaration not found"
				, _name.c_str()
				, _prototype.c_str() 
				);

			return 0;
		}

		const ResourceDesc & desc = it_find->second;
		const ConstString & type = Consts::get()->c_Entity;

		Entity * entity = ScriptEngine::get()
			->createEntityT<Entity>( _name, type, _tag, _prototype, desc.pak, desc.path );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager::createEntity: Can't create entity '%s''%s'"
				, _name.c_str()
				, _prototype.c_str() 
				); 

			return 0;
		}

		return entity;
	}
}