#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "Holder.h"
#	include "ScriptEngine.h"

#	include "Scene.h"
#	include "Layer.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT( Entity )
//////////////////////////////////////////////////////////////////////////
Entity::~Entity()
{
	Holder<ScriptEngine>::hostage();
}
//////////////////////////////////////////////////////////////////////////
void Entity::destroy()
{
	Holder<ScriptEngine>::hostage()
		->removeEntity( this );
}
//////////////////////////////////////////////////////////////////////////
bool Entity::addChildren( Node * _node )
{
	return Allocator2D::addChildren( _node );
};
//////////////////////////////////////////////////////////////////////////
bool Entity::_activate()
{
	Holder<ScriptEngine>::hostage()
		->callMethod( this, "activate", "()" );

	return true;
}