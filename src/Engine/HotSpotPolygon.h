#pragma once

#include "Kernel/HotSpot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotPolygon
        : public HotSpot
    {
        DECLARE_FACTORABLE( HotSpotPolygon );
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotPolygon();
        ~HotSpotPolygon() override;

    public:
        void setPolygon( const Polygon & _polygon );
        const Polygon & getPolygon() const;

    public:
        bool testPoint( const RenderContext * _context, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const override;
        bool testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    public:
        bool testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const override;

    public:
        void clearPoints();

    public:
        void getScreenPolygon( const RenderContext * _context, mt::box2f * const _bb, Polygon * const _screen ) const;

    protected:
        Polygon m_polygon;
        mutable Polygon m_polygonScreen;
        mutable Polygon m_polygonTemp;

        mutable bool m_invalidatePolygonWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotPolygon> HotSpotPolygonPtr;
    //////////////////////////////////////////////////////////////////////////
}
