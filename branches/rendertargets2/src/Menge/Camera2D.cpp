#	include "Camera2D.h"

#	include "Core/Holder.h"

#	include "Game.h"
#	include "RenderEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_target( NULL )
		, m_targetFollowing( false )
		, m_boundLeftUpper( 512.0f, 368.0f )
		, m_boundRightLower( 512.0f, 368.0f )
		, m_invalidateViewMatrix( true )
		, m_invalidateViewport( true )
		, m_parallax( 1.0f, 1.0f )
		, m_offset( 0.0f, 0.0f )
		, m_boundsEnabled( false )
		, m_renderViewport( 0.0f, 0.0f, 0.0f, 0.0f )
		, m_viewportSize( 0.0f, 0.0f )
		, m_cameraRevision(1)
	{
		mt::ident_m4( m_viewMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D::~Camera2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::loader( XmlElement * _xml )
	{
		Transformation2D::loader( _xml );
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

			if( len < 0.01f )
			{
				return;
			}

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

			if( m_boundsEnabled == true )
			{
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
			}

			pos.x = ::floorf( pos.x + 0.5f );
			pos.y = ::floorf( pos.y + 0.5f );

			const mt::vec2f& pp = getWorldPosition() - getLocalPosition();

			setLocalPosition( pos - pp );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateViewport()
	{
		m_invalidateViewport = false;
		++m_cameraRevision;

		const mt::vec2f & pos = getWorldPosition();

		//m_viewport.begin = pos;
		//m_viewport.end = pos + m_viewportSize;
		m_viewport.begin = pos - m_viewportSize * .5;
		m_viewport.begin.x = m_viewport.begin.x * m_parallax.x + m_offset.x;
		m_viewport.begin.y = m_viewport.begin.y * m_parallax.y + m_offset.y;
		m_viewport.end = m_viewport.begin + m_viewportSize;
		//m_viewport.end = pos + m_viewportSize * .5;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::invalidateViewport()
	{
		m_invalidateViewport = true;
		invalidateViewMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewportSize( const mt::vec2f & _size )
	{
		m_viewportSize = _size;

		invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setTarget( Node * _target )
	{
		m_target = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::enableTargetFollowing( bool _enable, float _force )
	{
		m_targetFollowing = _enable;
		m_followingForce = _force;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
	{
		m_boundsEnabled = true;
		m_boundLeftUpper = _leftUpper + m_viewportSize * 0.5f;
		m_boundRightLower = _rightLower - m_viewportSize * 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateViewMatrix()
	{
		m_invalidateViewMatrix = false;
		++m_cameraRevision;

		mt::ident_m4( m_viewMatrix );

		const Viewport & viewport = 
			this->getViewport();

		const Resolution& contentResolution = Game::hostage()
												->getContentResolution();
		m_viewMatrix.v0.x = m_viewportSize.x / contentResolution[0];
		m_viewMatrix.v1.y = m_viewportSize.y / contentResolution[1];
		m_viewMatrix.v3.x = viewport.begin.x;
		m_viewMatrix.v3.y = viewport.begin.y;
		m_viewMatrix = mt::inv_m4( m_viewMatrix );

		Holder<RenderEngine>::hostage()
			->setProjectionMatrix2D_( m_projectionMatrix, 0.0f, m_viewportSize.x, 0.0f, m_viewportSize.y, 0.0f, 1.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setParallax( const mt::vec2f& _parallax )
	{
		m_parallax = _parallax;
		invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Camera2D::getParallax() const
	{
		return m_parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setOffset( const mt::vec2f& _offset )
	{
		m_offset = _offset;
		invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Camera2D::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f& Camera2D::getProjectionMatrix()
	{
		if( isInvalidateViewport() == true )
		{
			updateViewMatrix();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f& Camera2D::getViewMatrix()
	{
		if( isInvalidateViewMatrix() == true )
		{
			updateViewMatrix();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getRenderViewport()
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::is3D() const 
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Camera2D::getViewportSize() const 
	{
		return m_viewportSize;
	}
	//////////////////////////////////////////////////////////////////////////

}
