#	pragma once

#	include "PrototypeManager.h"

#	include "Config/Typedef.h"

#	include <Core/Holder.h>
#	include "Core/ConstString.h"

namespace Menge
{
	class Entity;

	class EntityManager
		: public PrototypeManager
		, public Holder<EntityManager>
	{
	public:
		Entity * createEntity( const ConstString & _name, const ConstString & _prototype );
	};
}