#	include "Camera2D.h"

#	include "Core/Holder.h"

#	include "Game.h"
#	include "RenderEngine.h"

#	include "LogEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_target(0)    
		, m_cameraRevision(1)    
		, m_offsetProvider(0)
		, m_invalidateMatrix(true)
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

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_()
	{
		m_invalidateMatrix = false;

		RenderEngine::get()
			->makeViewMatrixFromViewport( m_viewMatrix, m_viewport );

		RenderEngine::get()
			->makeProjectionOrthogonalFromViewport( m_projectionMatrix, m_viewport );
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

		const mt::vec3f & wp = m_target->getWorldPosition();

		mt::vec2f vp_size;
		m_viewport.getSize( vp_size );

		mt::vec2f camera_wp = wp.to_vec2f() - vp_size * 0.5f;

		if( m_offsetProvider )
		{
			const mt::vec3f & lp = m_offsetProvider->getLocalPosition();

			camera_wp += lp.to_vec2f();
		}

		mt::vec3f v3_camera_wp(camera_wp.x, camera_wp.y, 0.f);
		this->setLocalPosition( v3_camera_wp );
	}
}
