#pragma once

#include "Kernel/HotSpot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotCircle
        : public HotSpot
    {
        DECLARE_FACTORABLE( HotSpotCircle );
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotCircle();
        ~HotSpotCircle() override;

    public:
        void setRadius( float _radius );
        float getRadius() const;

        void setEllipse( float _ellipse );
        float getEllipse() const;

    protected:
        bool testPoint( const RenderContext * _context, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const override;
        bool testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    protected:
        bool testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const override;

    protected:
        void getScreenBoundingBox( const RenderContext * _context, mt::box2f * const _bb ) const;

    protected:
        float m_radius;
        float m_ellipse;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotCircle> HotSpotCirclePtr;
    //////////////////////////////////////////////////////////////////////////
}
