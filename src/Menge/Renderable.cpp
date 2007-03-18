#	include "Renderable.h"
#	include "ObjectImplement.h"

#	include "ObjectForeach.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Renderable);
//////////////////////////////////////////////////////////////////////////
Renderable::Renderable()
: m_hide(false)
{

}
//////////////////////////////////////////////////////////////////////////
const Viewport & Renderable::updateViewport( const Viewport & _viewPort )
{
	return _viewPort;
}
//////////////////////////////////////////////////////////////////////////
bool Renderable::render( const Viewport & _viewPort )
{
	if( m_active == false )
	{
		return false;
	}

	if( m_hide == true )
	{
		return false;
	}

	if( isVisible( _viewPort ) == false )
	{
		return false;
	}

	compile();

	mt::mat3f rwm = getWorldMatrix();

	rwm.v2.v2 -= _viewPort.begin;

	_render( rwm, _viewPort );

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Renderable::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
{
	// Empty;
}
//////////////////////////////////////////////////////////////////////////
void Renderable::hide(bool value)
{
	m_hide = value;
}
//////////////////////////////////////////////////////////////////////////
bool Renderable::isVisible(const Viewport & _viewPort)
{
	// Empty;
	return true;
}