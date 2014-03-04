#	include "Layer2DIsometric.h"

#	include "Kernel/Scene.h"

#	include "Camera2D.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2DIsometric::Layer2DIsometric()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DIsometric::calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node * _node ) const
	{
		const mt::vec3f & wp = _node->getWorldPosition();

		const mt::mat4f & vm = _camera->getCameraViewMatrix();

		mt::vec3f sp;
		mt::mul_m4_v3( sp, vm, wp );

		_screen.x = sp.x;
		_screen.y = sp.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DIsometric::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		//const mt::mat4f & camera_vm = _camera->getCameraViewMatrix();
		//const mt::mat4f & camera_pm = _camera->getCameraProjectionMatrix();
		//const Viewport & renderport = _camera->getCameraRenderport();
		//const ConstString & renderTarget = _camera->getRenderTarget();
		//bool isOrthogonalProjection = _camera->isOrthogonalProjection();

		//mt::mat4f camera_vm_inv;
		//mt::inv_m4( camera_vm_inv, camera_vm );

		//const mt::vec2f & size = this->getSize();
		//		
		//float base_x = floorf( camera_vm_inv.v3.x * m_parallax.x / size.x );
		//float base_y = floorf( camera_vm_inv.v3.y * m_parallax.y / size.y );

		//for( size_t i = 0; i != 4; ++i )
		//{
		//	RenderCamera & rc = m_camera[i];

		//	float layer_x = base_x * size.x;
		//	float layer_y = base_y * size.y;

		//	layer_x += size.x * float(i % 2);
		//	layer_y += size.y * float(i / 2);

		//	mt::mat4f wm;
		//	mt::make_translation_m4( wm, layer_x, layer_y, 0.f );

		//	mt::mat4f vm;
		//	vm = camera_vm;
		//	vm.v3.x *= m_parallax.x;
		//	vm.v3.y *= m_parallax.y;

		//	rc.initialize( wm, camera_pm, vm, renderport, renderTarget, isOrthogonalProjection );

		//	Layer::render( _viewport, &rc, _debugMask );
		//}
	}
}
