#pragma once

#include "HotSpot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotGlobal
        : public HotSpot
    {
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotGlobal();
        ~HotSpotGlobal() override;

    public:
        bool testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
        bool testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotGlobal> HotSpotGlobalPtr;
    //////////////////////////////////////////////////////////////////////////
}
