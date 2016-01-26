#	include "RenderCamera.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderCamera::RenderCamera()
		: m_isOrthonalProjection(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::initialize( const mt::mat4f & _pm, const mt::mat4f & _vm, const Viewport & _renderport, bool _isOrthogonalProjection )
	{
		m_projectionMatrix = _pm;
		mt::inv_m4( m_projectionMatrixInv, m_projectionMatrix );

		m_viewMatrix = _vm;
		mt::inv_m4( m_viewMatrixInv, m_viewMatrix );

		m_renderport = _renderport;
				
		m_isOrthonalProjection = _isOrthogonalProjection;

		Viewport rp_vm;
		mt::mul_v2_m4( rp_vm.begin, m_renderport.begin, m_viewMatrixInv );
		mt::mul_v2_m4( rp_vm.end, m_renderport.end, m_viewMatrixInv );

		mt::box2f bb_vp;
		rp_vm.toBBox(bb_vp);

		mt::mul_v2_m4( m_bboxWM.minimum, bb_vp.minimum, m_viewMatrix );
		mt::mul_v2_m4( m_bboxWM.maximum, bb_vp.maximum, m_viewMatrix );

		mt::mul_m4_m4( m_viewProjectionMatrix, m_viewMatrix, m_projectionMatrix );
	}
}
