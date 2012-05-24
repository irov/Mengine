#	include "Camera3D.h"

#	include "RenderEngine.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_invalidateMatrix(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraInterest( const mt::vec3f & _pos )
	{
		m_cameraInterest = _pos;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraFOV( float _fov )
	{
		m_cameraFOV = _fov;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraAspect( float _aspect )
	{
		m_cameraAspect = _aspect;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::updateMatrix_() const
	{
		m_invalidateMatrix = false;


		m_viewportWM = m_viewport;

		//const mt::mat4f & wm = this->getWorldMatrix();

		//mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
		//mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );

		const mt::vec3f & wm_pos = this->getWorldPosition();

		mt::make_lookat_m4( m_viewMatrixWM, wm_pos, m_cameraInterest, mt::vec3f(0.f, 1.f, 0.f) );

		RenderEngine::get()
			->makeProjectionPerspective( m_projectionMatrixWM, m_cameraFOV, m_cameraAspect, 1.f, 10000.f );

		//mt::make_perspective_projection_m4( m_projectionMatrixWM, m_cameraFOV, m_cameraAspect, 1.f, 10000.f );

		//mt::mul_m4_m4( camera.vp, camera.view, camera.projection );

		//RenderEngine::get()
		//	->makeViewMatrixFromViewport( m_viewMatrix, m_viewportWM );

		//RenderEngine::get()
		//	->makeProjectionOrthogonalFromViewport( m_projectionMatrix, m_viewportWM );
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera3D::getViewport() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewportWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getProjectionMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrixWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixWM;
	}
}
