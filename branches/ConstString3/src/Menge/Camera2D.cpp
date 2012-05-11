#	include "Camera2D.h"

#	include "Core/Holder.h"

#	include "Game.h"
#	include "RenderEngine.h"

#	include "LogEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_invalidateMatrix(true)
		, m_cameraRevision(1)    	
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

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_() const
	{
		m_invalidateMatrix = false;
		
		const mt::mat4f & wm = this->getWorldMatrix();
				
		mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );

		RenderEngine::get()
			->makeViewMatrixFromViewport( m_viewMatrix, m_viewportWM );

		RenderEngine::get()
			->makeProjectionOrthogonalFromViewport( m_projectionMatrix, m_viewportWM );
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getViewport() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewportWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getProjectionMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}
}
