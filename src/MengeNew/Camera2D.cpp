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
		, m_boundLeftUpper( 512.0f, 368.0f )
		, m_boundRightLower( 512.0f, 368.0f )
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
			float len = dir.length();
			if( len < 0.01f ) return;
			dir /= len;

			float force = m_followingForce *  len ;
			float way = force * _timing * 0.001f;
			if( way > len )
			{
				pos = tpos;
			}
			else
			{
				pos += dir * way;
			}

			if( pos.x < m_boundLeftUpper.x )
			{
				pos.x = m_boundLeftUpper.x;
			}
			else if( pos.x > m_boundRightLower.x )
			{
				pos.x = m_boundRightLower.x;
			}
			if( pos.y < m_boundLeftUpper.y )
			{
				pos.y = m_boundLeftUpper.y;
			}
			else if( pos.y > m_boundRightLower.y )
			{
				pos.y = m_boundRightLower.y;
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

		m_viewport.begin = pos - m_viewportSize * .5f;
		m_viewport.end = pos + m_viewportSize * .5f;
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
	void Camera2D::setTarget( Node *_target)
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
	void Camera2D::setBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
	{
		m_boundLeftUpper = _leftUpper + m_viewportSize * 0.5f;
		m_boundRightLower = _rightLower - m_viewportSize * 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
}