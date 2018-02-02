#	include "RenderCameraOrthogonal.h"

#	include "Interface/ApplicationInterface.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderCameraOrthogonal::RenderCameraOrthogonal()
		: m_cameraPosition(0.f, 0.f, 0.f)
		, m_cameraDirection(0.f, 0.f, 1.f)
		, m_cameraUp(0.f, 1.f, 0.f)
		, m_cameraRightSign(1.f)
		, m_cameraNear( -10000.f )
		, m_cameraFar( 10000.f )
		, m_proxyViewMatrix( false )
		, m_fixedOrthogonalViewport( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraPosition( const mt::vec3f & _pos )
	{
		m_cameraPosition = _pos;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraDirection( const mt::vec3f & _dir )
	{
		m_cameraDirection = _dir;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraUp( const mt::vec3f & _up )
	{
		m_cameraUp = _up;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraRightSign( float _rightSign )
	{
		m_cameraRightSign = _rightSign;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraNear( float _near )
	{
		m_cameraNear = _near;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setCameraFar( float _far )
	{
		m_cameraFar = _far;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setOrthogonalViewport( const Viewport & _viewport )
	{
		m_orthogonalViewport = _viewport;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderCameraOrthogonal::getOrthogonalViewport() const
	{
		return m_orthogonalViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setFixedOrthogonalViewport( bool _value )
	{
		m_fixedOrthogonalViewport = _value;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderCameraOrthogonal::getFixedOrthogonalViewport() const
	{
		return m_fixedOrthogonalViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::setProxyViewMatrix( bool _value )
	{
		m_proxyViewMatrix = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderCameraOrthogonal::getProxyViewMatrix() const
	{
		return m_proxyViewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::_updateViewMatrix() const
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		if( m_proxyViewMatrix == false )
		{
			mt::vec3f wm_position;
			mt::mul_v3_v3_m4( wm_position, m_cameraPosition, wm );

			mt::vec3f wm_direction;
			mt::mul_v3_v3_m4_r( wm_direction, m_cameraDirection, wm );

			mt::vec3f wm_up;
			mt::mul_v3_v3_m4_r( wm_up, m_cameraUp, wm );

			RENDER_SERVICE()
				->makeViewMatrixLookAt( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

			mt::inv_m4_m4( m_viewMatrixInv, m_viewMatrix );
		}
		else
		{
			mt::inv_m4_m4( m_viewMatrix, wm );

			m_viewMatrixInv = wm;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraOrthogonal::_updateProjectionMatrix() const
	{
		Viewport renderViewport;

		if( m_fixedOrthogonalViewport == false )
		{
			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE()
				->getGameViewport( gameViewportAspect, gameViewport );

			Viewport renderViewportWM;

			const mt::mat4f & wm = this->getWorldMatrix();

			mt::mul_v2_v2_m4( renderViewportWM.begin, m_orthogonalViewport.begin, wm );
			mt::mul_v2_v2_m4( renderViewportWM.end, m_orthogonalViewport.end, wm );

			renderViewportWM.clamp( gameViewport );

			mt::mat4f wm_inv;
			mt::inv_m4_m4( wm_inv, wm );

			mt::mul_v2_v2_m4( renderViewport.begin, renderViewportWM.begin, wm_inv );
			mt::mul_v2_v2_m4( renderViewport.end, renderViewportWM.end, wm_inv );
		}
		else
		{
			renderViewport = m_orthogonalViewport;
		}

		RENDER_SERVICE()
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, m_cameraNear, m_cameraFar );

		mt::inv_m4_m4( m_projectionMatrixInv, m_projectionMatrix );
	}
}
