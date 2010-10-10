#	pragma once

#	include "Factory.h"

#	include "Config/Typedef.h"

#	include <Core/Holder.h>
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class Entity;

	struct PrototypeDesc
	{
		ConstString pak;
		ConstString path;
	};

	class EntityManager
		: public Holder<EntityManager>
	{
	public:
		EntityManager();
		~EntityManager();

	public:
		void addPrototype( const ConstString & _type, const PrototypeDesc & _desc );
		
		bool getPrototypeDesc( const ConstString & _type, PrototypeDesc & _desc );

	public:
		Entity * createEntity( const ConstString & _name, const ConstString & _prototype );

	protected:
		typedef std::map<ConstString, PrototypeDesc> TMapDescriptionEntities;
		TMapDescriptionEntities m_descriptions;

		typedef std::map<ConstString, TBlobject> TMapEntitiesData;
		TMapEntitiesData m_entitiesData;

	private:
		bool setupEntityDesk_( Entity * _entity, const PrototypeDesc & _desc );
		bool loadEntityData_( const ConstString & _type, const PrototypeDesc & _desc, TBlobject & _data );
	};
}