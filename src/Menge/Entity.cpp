#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Entity )
	//////////////////////////////////////////////////////////////////////////
	Entity::Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Entity::~Entity()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		if( Holder<ScriptEngine>::hostage()
			->hasMethod( this, "onActivate" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callMethod( this, "onActivate" );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		if( Holder<ScriptEngine>::hostage()
			->hasMethod( this, "onLoader" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callMethod( this, "onLoader" );
		}
	}
}
