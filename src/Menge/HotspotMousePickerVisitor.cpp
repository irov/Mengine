#   include "HotspotMousePickerVisitor.h"

#   include "Layer2D.h"
#   include "Layer2DIsometric.h"

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

		this->test_();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotspotMousePickerVisitor::accept( Layer2DIsometric * _visited )
	{
		(void)_visited;

		this->test_();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotspotMousePickerVisitor::test_()
	{
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
				mt::mul_v2_m4( p_vm, p, vm_inv );

				p_vm -= viewport.begin;

				m_result = m_hotspot->testPoint( p_vm );
			}break;
		case EAT_RADIUS:
			{
				float radius = m_arrow->getRadius();

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, m_point, vm_inv );

				p_vm -= viewport.begin;

				m_result = m_hotspot->testRadius( p_vm, radius );
			}break;
		case EAT_POLYGON:
			{
				const Polygon & polygon = m_arrow->getPolygon();

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, m_point, vm_inv );

				p_vm -= viewport.begin;

				m_result = m_hotspot->testPolygon( p_vm, polygon );
			}break;
		}
    }
}