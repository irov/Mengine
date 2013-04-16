#   include "MousePicker.h"

#   include "Arrow.h"
#   include "HotSpot.h"

#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
    MousePicker::MousePicker( HotSpot * _layerspaceHotspot, RenderCameraInterface * _camera, const mt::vec2f & _point, Arrow * _arrow )
        : m_layerspaceHotspot(_layerspaceHotspot)
        , m_camera(_camera)
        , m_point(_point)
        , m_arrow(_arrow)
        , m_result(false)
    {
    }
    bool MousePicker::test( Layer * _layer )
    {
        _layer->visit( this );

        return m_result;
    }
    //////////////////////////////////////////////////////////////////////////
    void MousePicker::accept( Layer2D * _visited )
    {
        (void)_visited;

        mt::mat4f lwm = m_layerspaceHotspot->getWorldMatrix();

        mt::vec2f cp;
        m_layerspaceHotspot->getCameraPosition( m_camera, cp );

        lwm.v3.x = cp.x;
        lwm.v3.y = cp.y;

        //const mt::mat3f & awm = _arrow->getWorldMatrix();
        //const mt::mat3f & acm = _arrow->getClickMatrix();

        mt::mat4f click_wm;
        mt::ident_m4(click_wm);

        //= awm;
        click_wm.v3.x = m_point.x;
        click_wm.v3.y = m_point.y;
        click_wm.v3.z = 0.f;
        //mt::mul_m3_m3( click_wm, awm, acm );

        //const Polygon & screenPoly = _arrow->getPolygon();

        //bool result = _layerspaceHotspot->testPolygon( lwm, screenPoly, click_wm );

        m_result = m_layerspaceHotspot->testArrow( lwm, m_arrow, click_wm );
    }
}