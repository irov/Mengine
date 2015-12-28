#	include "Camera2D.h"

#	include "Interface/ApplicationInterface.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_cameraPosition(0.f, 0.f, 0.f)
		, m_cameraDir(0.f, 0.f, 1.f)
		, m_cameraUp(0.f, 1.f, 0.f)
		, m_cameraRightSign(1.f)
		, m_zNear( -40000.f )
		, m_zFar( 10000.f )
		, m_observerChangeWindowResolution( nullptr )
		, m_fixedRenderport( false )
		, m_invalidateProjectionMatrix( true )
		, m_invalidateMatrix( true )
		, m_invalidateBB( true )
		, m_orthogonalProjection( true )
	{
		mt::ident_m4( m_viewMatrix );
		mt::ident_m4( m_viewMatrixInv );
		mt::ident_m4( m_projectionMatrix );
		mt::ident_m4( m_projectionMatrixInv );
		
		mt::ident_box( m_bboxWM );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
        if( Node::_activate() == false )
		{
			return true;
		}

		m_observerChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &Camera2D::notifyChangeWindowResolution );

		this->invalidateViewMatrix_();
		this->invalidateProjectionMatrix_();

		//this->setDirection( {0.f, 1.f, 1.f}, {0.f, 1.f, 0.f} );
		//this->setLocalPosition( {100.f, 100.f, 100.f} );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_deactivate()
	{
        Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, m_observerChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setCameraPosition( const mt::vec3f & _pos )
	{
		m_cameraPosition = _pos;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setCameraDir( const mt::vec3f & _dir )
	{
		m_cameraDir = _dir;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setCameraUp( const mt::vec3f & _up )
	{
		m_cameraUp = _up;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setCameraRightSign( float _rightSign )
	{
		m_cameraRightSign = _rightSign;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setRenderport( const Viewport & _viewport )
	{
		m_renderport = _viewport;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setFixedRenderport( bool _value )
	{
		m_fixedRenderport = _value;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::getFixedRenderport() const
	{
		return m_fixedRenderport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setOrthogonalProjection( bool _orthogonalProjection )
	{
		m_orthogonalProjection = _orthogonalProjection;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec3f wm_position;
		mt::mul_v3_m4( wm_position, m_cameraPosition, wm );

		mt::vec3f wm_direction;
		mt::mul_v3_m4_r( wm_direction, m_cameraDir, wm );

		mt::vec3f wm_up;
		mt::mul_v3_m4_r( wm_up, m_cameraUp, wm );

		RENDER_SERVICE( m_serviceProvider )
			->makeViewMatrixLookAt( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

		mt::inv_m4( m_viewMatrixInv, m_viewMatrix );

		//mt::vec3f pos( 0.f, 0.f, 0.f );
		//mt::vec3f dir( 0.f, 1.f, 1.f );
		//mt::vec3f up( 0.f, 1.f, 0.f );
		////mt::vec3f target( 1124.f, 492.f, 0.f );
		////mt::vec3f target( 0.f, 0.f, 0.f );
		////mt::vec3f dir;
		////mt::dir_v3_v3( dir, target, pos );

		//mt::mat4f vm;
		//mt::ident_m4( vm );
		//mt::make_lookat_m4( vm, pos, dir, up, 1.f );

		//m_viewMatrix = vm;
		//mt::inv_m4( m_viewMatrix, vm );

		//mt::inv_m4( m_viewMatrixInv, m_viewMatrix );

		//const mt::mat4f & wm = this->getWorldMatrix();
		//mt::inv_m4( m_worldMatrixInv, wm );

		//const mt::mat4f & wm = this->getWorldMatrix();

		//mt::inv_m4( m_viewMatrix, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateBBoxWM_() const
	{
		m_invalidateBB = false;

		const Viewport & rp = this->getCameraRenderport();

		const mt::mat4f & wm = this->getWorldMatrix();

		Viewport rp_vm;
		mt::mul_v2_m4( rp_vm.begin, rp.begin, wm );
		mt::mul_v2_m4( rp_vm.end, rp.end, wm );

		mt::box2f bb_vp;
		rp_vm.toBBox(bb_vp);

		const mt::mat4f & vm = this->getCameraViewMatrix();

		mt::mul_v2_m4( m_bboxWM.minimum, bb_vp.minimum, vm );
		mt::mul_v2_m4( m_bboxWM.maximum, bb_vp.maximum, vm );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateProjectionMatrix_() const
	{
		m_invalidateProjectionMatrix = false;

		Viewport renderViewport;

		if( m_fixedRenderport == false )
		{
			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE(m_serviceProvider)
				->getGameViewport( gameViewportAspect, gameViewport );

			Viewport renderViewportWM;

			const mt::mat4f & wm = this->getWorldMatrix();

			mt::mul_v2_m4( renderViewportWM.begin, m_renderport.begin, wm );
			mt::mul_v2_m4( renderViewportWM.end, m_renderport.end, wm );
			
			renderViewportWM.clamp( gameViewport );

			mt::mat4f wm_inv;
			mt::inv_m4( wm_inv, wm );

			mt::mul_v2_m4( renderViewport.begin, renderViewportWM.begin, wm_inv );
			mt::mul_v2_m4( renderViewport.end, renderViewportWM.end, wm_inv );
		}
		else
		{
			renderViewport = m_renderport;
		}

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, m_zNear, m_zFar );

		mt::inv_m4( m_projectionMatrixInv, m_projectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateProjectionMatrix_();
	}
}
