#	pragma once

#	include <Core/Holder.h>

#	include "ConstManager.h"

namespace Menge
{
	class Entity;

	struct EntityDesc
	{
		ConstString pak;
		String path;
	};

	class EntityManager
		: public Holder<EntityManager>
	{
	public:
		EntityManager();
		~EntityManager();

	public:
		void registerEntityType( const ConstString & _name, const EntityDesc & _desc );

	public:
		Entity * createEntity( const ConstString & _name );

	protected:
		typedef std::map<ConstString, Entity *> TMapEntities;
		TMapEntities m_entities;

		typedef std::map<ConstString, EntityDesc> TMapDescriptionEntities;
		TMapDescriptionEntities m_descriptions;

		typedef std::map<ConstString, Blobject> TMapEntitiesData;
		TMapEntitiesData m_mapEntitiesData;

	protected:
		Entity * createEntity_( const ConstString & _name );
		bool setupEntity_( const EntityDesc & _desc, Entity * _entity );
		TMapEntitiesData::iterator getEntityData_( const ConstString & _type, const EntityDesc & _desc );
	};
}