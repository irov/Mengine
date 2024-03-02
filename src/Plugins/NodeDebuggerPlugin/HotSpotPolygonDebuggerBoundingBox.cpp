#include "HotSpotPolygonDebuggerBoundingBox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonDebuggerBoundingBox::HotSpotPolygonDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonDebuggerBoundingBox::~HotSpotPolygonDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygonDebuggerBoundingBox::_getBoundingBox( const HotSpotPolygonPtr & _node, mt::box2f * _bb ) const
    {
        const Polygon & polygon = _node->getPolygon();

        const VectorPoints & points = polygon.getPoints();

        if( points.empty() == true )
        {
            return false;
        }

        const TransformationInterface * transformation = _node->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        mt::box2f bb;
        mt::box2_insideout( &bb );

        for( const mt::vec2f & v : points )
        {
            mt::vec2f wmp_it;
            mt::mul_v2_v2_m4( &wmp_it, v, wm );

            mt::box2_add_internal_point( &bb, wmp_it );
        }

        *_bb = bb;

        return true;
    };
}
