#	include "RenderCamera.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderCamera::RenderCamera()
		: m_isOrthonalProjection(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCamera::initialize( const mt::mat4f & _wm, const mt::mat4f & _pm, const mt::mat4f & _vm, const Viewport & _renderport, const ConstString & _target, bool _isOrthogonalProjection )
	{
		m_worldMatrix = _wm;
		m_projectionMatrix = _pm;
		m_viewMatrix = _vm;

		m_renderport = _renderport;
		m_renderTarget = _target;

		m_isOrthonalProjection = _isOrthogonalProjection;
	}
}
