#pragma once

#include "HotSpot.h"

namespace Mengine
{
    class HotSpotCircle
        : public HotSpot
    {
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
        bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const override;
        bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    protected:
        float m_radius;
        float m_ellipse;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpotCircle, HotSpot> HotSpotCirclePtr;
    //////////////////////////////////////////////////////////////////////////
}
