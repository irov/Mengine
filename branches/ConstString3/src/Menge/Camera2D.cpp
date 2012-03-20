#	include "Camera2D.h"

#	include "Core/Holder.h"

#	include "Game.h"
#	include "RenderEngine.h"

#	include "LogEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_parallax(1.0f, 1.0f)
		, m_target(0)    
		, m_invalidateViewport(true)
		, m_cameraRevision(1)    
		, m_offsetProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D::~Camera2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setRenderport( const Viewport & _renderport )
	{
		m_renderport = _renderport;

		this->invalidateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setParallax( const mt::vec2f& _parallax )
	{
		m_parallax = _parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Camera2D::getParallax() const
	{
		return m_parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateViewport_()
	{
		m_invalidateViewport = false;

		const mt::vec2f & pos = this->getWorldPosition();
		mt::vec2f renderport_size = m_renderport.getSize();

		mt::vec2f centerPosition = pos + renderport_size * 0.5;

		const mt::vec2f & scale = this->getScale();
		mt::vec2f half_scale_renderport_size;
		half_scale_renderport_size.x = renderport_size.x * scale.x * 0.5f;
		half_scale_renderport_size.y = renderport_size.y * scale.y * 0.5f;

		m_viewport.begin = centerPosition - half_scale_renderport_size;
		m_viewport.end = centerPosition + half_scale_renderport_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setTargetNode( Node * _target )
	{
		m_target = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setTargetOffset( Node * _offsetProvider )
	{
		m_offsetProvider = _offsetProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_update( float _timing )
	{
		if( m_target == NULL )
		{
			return;
		}

		const mt::vec2f & wp = m_target->getWorldPosition();

		mt::vec2f camera_wp;		
		mt::vec2f renderport_size = m_renderport.getSize();

		camera_wp = wp - renderport_size * 0.5f;

		if( m_offsetProvider )
		{
			const mt::vec2f & lp = m_offsetProvider->getLocalPosition();

			camera_wp += lp;
		}

		this->setLocalPosition( camera_wp );
	}
}
