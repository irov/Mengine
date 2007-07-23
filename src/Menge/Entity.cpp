#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity )
	//////////////////////////////////////////////////////////////////////////
	Entity::~Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		if( Holder<ScriptEngine>::hostage()
			->hasMethod( this, "_active" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callMethod( this, "_activate" );
		}

		return true;
	}
}