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
		void registerEntityType( const ConstString & _type, const EntityDesc & _desc );

	public:
		Entity * createEntity( const ConstString & _type );

	protected:
		typedef std::map<ConstString, EntityDesc> TMapDescriptionEntities;
		TMapDescriptionEntities m_descriptions;

		typedef std::map<ConstString, TBlobject> TMapEntitiesData;
		TMapEntitiesData m_mapEntitiesData;

	protected:
		Entity * createEntity_( const ConstString & _name );
		bool setupEntity_( Entity * _entity, const EntityDesc & _desc );
		bool setupEntity_( Entity * _entity, const TBlobject::value_type * _buffer, TBlobject::size_type _size );

		const TBlobject * getEntityData_( const ConstString & _type, const EntityDesc & _desc );

	};
}