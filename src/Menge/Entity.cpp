#	include "Entity.h"
#	include "ObjectImplement.h"

#	include "Scene.h"
#	include "Layer.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT( Entity )
//////////////////////////////////////////////////////////////////////////
void Entity::setScene( Scene * _scene )
{
	m_scene = _scene;
}
//////////////////////////////////////////////////////////////////////////
void Entity::setLayer( const std::string & _name )
{
	if( m_parent )
	{
		m_parent->removeChildren( this );
	}

	Layer * layer = m_scene->getChildrenT<Layer>( _name );

	if( layer )
	{
		layer->addChildren( this );
	}
}
//////////////////////////////////////////////////////////////////////////
bool Entity::_activate()
{
	return true;
}