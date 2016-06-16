#	include "RenderCameraHelper.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		void screenToWorldPosition( const RenderCameraInterface * _renderCamera, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint )
		{
			const mt::mat4f & pm = _renderCamera->getCameraProjectionMatrix();

			mt::mat4f pm_inv;
			mt::inv_m4_m4( pm_inv, pm );

			mt::vec2f p1 = _screenPoint * 2.f - mt::vec2f( 1.f, 1.f );
			p1.y = -p1.y;

			mt::vec2f p_pm;
			mt::mul_v2_m4( p_pm, p1, pm_inv );

			const mt::mat4f & vm = _renderCamera->getCameraViewMatrix();

			mt::mat4f vm_inv;
			mt::inv_m4_m4( vm_inv, vm );

			mt::vec2f p = p_pm;

			mt::vec2f p_vm;
			mt::mul_v2_m4( p_vm, p, vm_inv );

			_worldPoint = p_vm;
		}
		//////////////////////////////////////////////////////////////////////////
		void screenToWorldDelta( const RenderCameraInterface * _renderCamera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha )
		{
			const mt::mat4f & pm = _renderCamera->getCameraProjectionMatrix();

			mt::mat4f pm_inv;
			mt::inv_m4_m4( pm_inv, pm );

			mt::vec2f p1 = (_screenPoint + _screenDeltha) * 2.f - mt::vec2f( 1.f, 1.f );
			p1.y = -p1.y;

			mt::vec2f p_pm;
			mt::mul_v2_m4( p_pm, p1, pm_inv );

			mt::vec2f p2 = (_screenPoint)* 2.f - mt::vec2f( 1.f, 1.f );
			p2.y = -p2.y;

			mt::vec2f p_pm_base;
			mt::mul_v2_m4( p_pm_base, p2, pm_inv );

			mt::vec2f deltha = p_pm - p_pm_base;

			_worldDeltha = deltha;
		}
	}
}
