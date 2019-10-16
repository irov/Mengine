#pragma once

#include "HotSpot.h"

namespace Mengine
{
    class HotSpotGlobal
        : public HotSpot
    {
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotGlobal();
        ~HotSpotGlobal() override;

    public:
        bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const override;
        bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotGlobal> HotSpotGlobalPtr;
    //////////////////////////////////////////////////////////////////////////
}
