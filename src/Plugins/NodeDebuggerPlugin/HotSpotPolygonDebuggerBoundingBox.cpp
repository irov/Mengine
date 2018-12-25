#include "HotSpotPolygonDebuggerBoundingBox.h"

namespace Mengine
{
    bool HotSpotPolygonDebuggerBoundingBox::_getBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb )
    {
        const Polygon & polygon = _node->getPolygon();

        const VectorPoints & points = polygon.getPoints();

        if( points.empty() == true )
        {
            return false;
        }

        const mt::mat4f & wm = _node->getWorldMatrix();

        mt::box2f bb;
        mt::insideout_box( bb );

        for( const mt::vec2f & v : points )
        {
            mt::vec2f wmp_it;
            mt::mul_v2_v2_m4( wmp_it, v, wm );

            mt::add_internal_point( bb, wmp_it );
        }

        *_bb = bb;

        return true;
    };
}
