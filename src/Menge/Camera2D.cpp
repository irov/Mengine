#	include "Camera2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera2D );
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_target( NULL )
		, m_targetFollowing( false )
	{}
	//////////////////////////////////////////////////////////////////////////
	Camera2D::~Camera2D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::loader( XmlElement * _xml )
	{
		Allocator2D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_update( float _timing )
	{
		if( m_targetFollowing && m_target )
		{
			mt::vec2f pos = getWorldPosition();
			mt::vec2f tpos = m_target->getWorldPosition();
			mt::vec2f dir = tpos - pos;
			float slen = dir.sqrlength();

			float way = m_followingForce * _timing;
			if( way*way > slen )
			{
				pos = tpos;
			}
			else
			{
				pos += dir * way;
			}
			setLocalPosition( pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_changePivot()
	{
		Allocator2D::_changePivot();

		updateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateViewport()
	{
		const mt::vec2f & pos = getWorldPosition();

		m_viewport.begin = pos - m_viewportSize * .5;
		m_viewport.end = pos + m_viewportSize * .5;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getViewport() const
	{
		return m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewportSize( const mt::vec2f & _size )
	{
		m_viewportSize = _size;
		updateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setTarget(Menge::SceneNode2D *_target)
	{
		m_target = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::enableTargetFollowing( bool _enable, float _force )
	{
		m_targetFollowing = true;
		m_followingForce = _force;
	}
	//////////////////////////////////////////////////////////////////////////
}