#	pragma once

#	include "Factory.h"

#	include "Config/Typedef.h"

#	include <Core/Holder.h>
#	include "Core/ConstString.h"

#	include "ResourceManager.h"

#	include <map>

namespace Menge
{
	class Entity;

	class EntityManager
		: public Holder<EntityManager>
	{
	public:
		EntityManager();
		~EntityManager();

	public:
		void registerPrototype( const ResourceDesc & _desc );
		
	public:
		bool getPrototypeDesc( const ConstString & _type, ResourceDesc & _desc );

	public:
		Entity * createEntity( const ConstString & _name, const ConstString & _prototype, const ConstString & _tag );

	protected:
		typedef std::map<ConstString, ResourceDesc> TMapDescriptionEntities;
		TMapDescriptionEntities m_descriptions;

	//private:
		//bool setupEntityDesk_( Entity * _entity, const PrototypeDesc & _desc );
		//bool loadEntityData_( const ConstString & _type, const PrototypeDesc & _desc, TBlobject & _data );
	};
}