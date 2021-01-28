#pragma once

#include "HotSpot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotPolygon
        : public HotSpot
    {
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotPolygon();
        ~HotSpotPolygon() override;

    public:
        void setPolygon( const Polygon & _polygon );
        const Polygon & getPolygon() const;

    public:
        bool testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const override;
        bool testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    public:
        void clearPoints();

    public:
        void getScreenPolygon( const RenderContext * _context, const Resolution & _contentResolution, mt::box2f * const _bb, Polygon * const _screen ) const;

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
