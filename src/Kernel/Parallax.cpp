#	include "Parallax.h"

#	include "Logger/Logger.h"

#	include <math.h>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Parallax::Parallax()
		: m_parallaxFactor(1.f, 1.f, 1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Parallax::setParallaxFactor( const mt::vec3f & _parallax )
	{
		m_parallaxFactor = _parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Parallax::getParallaxFactor() const
	{
		return m_parallaxFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Parallax::render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		const mt::mat4f & camera_vm = _state->camera->getCameraViewMatrix();
		const mt::mat4f & camera_pm = _state->camera->getCameraProjectionMatrix();
		const Viewport & renderport = _state->camera->getCameraRenderport();

		bool isOrthogonalProjection = _state->camera->isOrthogonalProjection();

		mt::mat4f vm;
		vm = camera_vm;
		vm.v3.x *= m_parallaxFactor.x;
		vm.v3.y *= m_parallaxFactor.y;
		vm.v3.z *= m_parallaxFactor.z;

		m_parallaxCamera.initialize( camera_pm, vm, renderport, isOrthogonalProjection );

		RenderObjectState state;
		state.viewport = _state->viewport;
		state.camera = &m_parallaxCamera;
		state.clipplane = _state->clipplane;
		state.target = _state->target;

		Node::render( &state, _debugMask );
	}
}
