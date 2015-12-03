#	include "CameraIsometric.h"

#	include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	CameraIsometric::CameraIsometric()
		: m_cameraRevision(1)
		, m_zNear(-10000.f)
		, m_zFar(10000.f)
		, m_invalidateProjectionMatrix(true)
		, m_invalidateMatrix(true)
		, m_invalidateBB(true)
	{		
		mt::ident_m4( m_worldMatrix );
		mt::ident_m4( m_viewMatrix );
		mt::ident_m4( m_viewMatrixInv );
		mt::ident_m4( m_projectionMatrix );
		
		mt::ident_box( m_bboxWM );
	}
	//////////////////////////////////////////////////////////////////////////
	bool CameraIsometric::_activate()
	{
        if( Node::_activate() == false )
		{
			return true;
		}

		this->invalidateMatrix_();
		this->invalidateViewport_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::_deactivate()
	{
        Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::setRenderport( const Viewport & _viewport )
	{
		m_renderport = _viewport;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		const mt::vec3f & wp = this->getWorldPosition();

		//mt::vec3f pos( 0.f, 0.f, 1500.f );
		mt::vec3f pos( 0.f, 0.f, 1500.f );
		mt::vec3f dir( 0.f, 1.f, 1.f );
		//mt::vec3f target( 1124.f, 492.f, 0.f );
		//mt::vec3f target( 0.f, 0.f, 0.f );
		//mt::vec3f dir;
		//mt::dir_v3_v3( dir, target, pos );

		mt::mat4f vm;
		//mt::ident_m4( vm );
		mt::make_lookat_m4( vm, pos, dir, mt::vec3f( 0.f, 1.f, 0.f ), 1.f );

		m_viewMatrix = vm;

		mt::inv_m4( m_viewMatrixInv, m_viewMatrix );

		const mt::mat4f & wm = this->getWorldMatrix();
		mt::inv_m4( m_worldMatrixInv, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::updateBBoxWM_() const
	{
		m_invalidateBB = false;

		const Viewport & rp = this->getCameraRenderport();

		const mt::mat4f & vm_inv = this->getCameraViewMatrixInv();

		Viewport rp_vm;
		mt::mul_v2_m4( rp_vm.begin, rp.begin, vm_inv );
		mt::mul_v2_m4( rp_vm.end, rp.end, vm_inv );

		mt::box2f bb_vp;
		rp_vm.toBBox(bb_vp);

		const mt::mat4f & wm_inv = this->getCameraWorldMatrixInv();

		mt::mul_v2_m4( m_bboxWM.minimum, bb_vp.minimum, wm_inv );
		mt::mul_v2_m4( m_bboxWM.maximum, bb_vp.maximum, wm_inv );
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraIsometric::updateProjectionMatrix_() const
	{
		m_invalidateProjectionMatrix = false;
			
		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, m_renderport, m_zNear, m_zFar );

		//mt::ident_m4( m_projectionMatrix );

		mt::inv_m4( m_projectionMatrixInv, m_projectionMatrix );
	}
}
