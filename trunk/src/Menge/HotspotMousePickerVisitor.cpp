#   include "HotspotMousePickerVisitor.h"

#   include "Arrow.h"
#   include "HotSpot.h"

#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    HotspotMousePickerVisitor::HotspotMousePickerVisitor( HotSpot * _layerspaceHotspot, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const mt::vec2f & _point, Arrow * _arrow )
        : m_hotspot(_layerspaceHotspot)
		, m_viewport(_viewport)
        , m_camera(_camera)
        , m_point(_point)
        , m_arrow(_arrow)
        , m_result(false)
    {
    }
	//////////////////////////////////////////////////////////////////////////
    bool HotspotMousePickerVisitor::test( Layer * _layer )
    {
        _layer->visit( this );

        return m_result;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotspotMousePickerVisitor::accept( Layer2D * _visited )
    {
        (void)_visited;

  //      mt::mat4f lwm = m_layerspaceHotspot->getWorldMatrix();

  //      const mt::vec3f & cp = m_layerspaceHotspot->getWorldPosition();

		//lwm.v3.x = cp.x;
  //      lwm.v3.y = cp.y;

        //const mt::mat3f & awm = _arrow->getWorldMatrix();
        //const mt::mat3f & acm = _arrow->getClickMatrix();

        //mt::mat4f click_wm;
        //mt::ident_m4(click_wm);

        ////= awm;
        //click_wm.v3.x = m_point.x;
        //click_wm.v3.y = m_point.y;
        //click_wm.v3.z = 0.f;
        //mt::mul_m3_m3( click_wm, awm, acm );

        //const Polygon & screenPoly = _arrow->getPolygon();

        //bool result = _layerspaceHotspot->testPolygon( lwm, screenPoly, click_wm );
		//const mt::mat4f & vm = m_camera->getWorldM();

		const mt::mat4f & vm = m_camera->getCameraViewMatrix();

		mt::mat4f vm_inv;
		mt::inv_m4( vm_inv, vm );

		const Viewport & viewport = m_viewport->getViewport();

		EArrowType arrowType = m_arrow->getArrowType();

		switch( arrowType )
		{
		case EAT_POINT:
			{
				const mt::vec2f & pc = m_arrow->getPointClick();

				mt::vec2f p = m_point + pc;

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, p, vm );

				p_vm += viewport.begin;

				m_result = m_hotspot->testPoint( p_vm );
			}break;
		case EAT_RADIUS:
			{
				float radius = m_arrow->getRadius();

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, m_point, vm );

				p_vm += viewport.begin;

				m_result = m_hotspot->testRadius( p_vm, radius );
			}break;
		case EAT_POLYGON:
			{
				const Polygon & polygon = m_arrow->getPolygon();

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, m_point, vm );

				p_vm += viewport.begin;
				
				m_result = m_hotspot->testPolygon( p_vm, polygon );
			}break;
		}
    }
}