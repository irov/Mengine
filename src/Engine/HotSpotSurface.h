#pragma once

#include "Engine/HotSpotPolygon.h"

#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotSurface
        : public HotSpotPolygon
    {
        DECLARE_FACTORABLE( HotSpotSurface );
        DECLARE_VISITABLE( HotSpotPolygon );

    public:
        HotSpotSurface();
        ~HotSpotSurface() override;

    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _dispose() override;

    public:
        void getWorldBoundingBox( mt::box2f * const _bb ) const;

    protected:
        SurfacePtr m_surface;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotSurface> HotSpotSurfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
