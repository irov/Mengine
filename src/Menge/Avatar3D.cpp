#	include "Avatar3D.h"

#	include "ObjectImplement.h"

#	include "ActionMove.h"

#	include "ActionLook.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Avatar3D)
	//////////////////////////////////////////////////////////////////////////
	Avatar3D::Avatar3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::handleMouseMove( float _x, float _y, float _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::loader( TiXmlElement * _xml )
	{
		Actor3D::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::_update( float _timing)
	{
		Actor3D::_update(_timing);
		m_actionScheduler.update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::moveToPoint(const mt::vec3f& _dest)
	{
		m_actionScheduler.runAction( new ActionMove(this, _dest) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::lookAtPoint(const mt::vec3f& _dest)
	{
		m_actionScheduler.runAction( new ActionLook(this, _dest) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Avatar3D::_activate()
	{
		return Actor3D::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::_deactivate()
	{
		Actor3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Avatar3D::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
	{
		Actor3D::_render(_rwm, _camera);
	}
	//////////////////////////////////////////////////////////////////////////
}