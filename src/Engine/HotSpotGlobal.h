#pragma once

#include "HotSpot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotGlobal
        : public HotSpot
    {
        DECLARE_FACTORABLE( HotSpotGlobal );
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotGlobal();
        ~HotSpotGlobal() override;

    public:
        bool testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const override;
        bool testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    protected:
        bool testBounds( const RenderContext * _context, const Resolution & _contentResolution, float _left, float _right, float _top, float _bottom ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotGlobal> HotSpotGlobalPtr;
    //////////////////////////////////////////////////////////////////////////
}
