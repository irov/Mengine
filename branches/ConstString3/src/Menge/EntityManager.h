#	pragma once

#	include "Factory.h"

#	include "Config/Typedef.h"

#	include <Core/Holder.h>
#	include "Core/ConstString.h"

#	include <map>

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
		, public Factory
	{
	public:
		EntityManager();
		~EntityManager();

	public:
		void registerEntityType( const ConstString & _type, const EntityDesc & _desc );

	public:
		Entity * createEntity( const ConstString & _type );

	protected:
		void destroyObject( Factorable * _node ) override;
		Factorable * _createObject( const ConstString & _type ) override;

	protected:
		typedef std::map<ConstString, EntityDesc> TMapDescriptionEntities;
		TMapDescriptionEntities m_descriptions;

		typedef std::map<ConstString, TBlobject> TMapEntitiesData;
		TMapEntitiesData m_mapEntitiesData;

	private:
		Entity * createEntity_( const ConstString & _name );
		bool setupEntityDesk_( Entity * _entity, const EntityDesc & _desc );
		bool setupEntityData_( Entity * _entity, const TBlobject & _buffer );

		const TBlobject * getEntityData_( const ConstString & _type, const EntityDesc & _desc );

	};
}