#	include "Renderable.h"
#	include "ObjectImplement.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Renderable);
//////////////////////////////////////////////////////////////////////////
Renderable::Renderable()
: m_hide(false)
{

}
//////////////////////////////////////////////////////////////////////////
void Renderable::render()
{
	if( m_active == false )
	{
		return;
	}

	if( m_hide == true )
	{
		return;
	}

	compile();

	_render();
}
//////////////////////////////////////////////////////////////////////////
void Renderable::_render()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void Renderable::hide(bool value)
{
	m_hide = value;
}