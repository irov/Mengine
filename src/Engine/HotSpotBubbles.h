#pragma once

#include "HotSpot.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotBubbles
        : public HotSpot
    {
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotBubbles();
        ~HotSpotBubbles() override;

    public:
        uint32_t addBubble( const mt::vec2f & _pos, float _radius, float _ellipse, bool _outward );

    protected:
        bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const override;
        bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        void invalidatemBubbleWM_() const;
        void updateBubbleWM_() const;

    protected:
        struct Bubble
        {
            UniqueId id;
            mt::vec2f pos;
            mutable mt::vec2f pos_wm;
            float radius;
            float ellipse;
            bool outward;
        };

        typedef Vector<Bubble> VectorBubbles;
        VectorBubbles m_bubbles;

        mutable bool m_invalidateBubbleWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotBubbles> HotSpotBubblesPtr;
    //////////////////////////////////////////////////////////////////////////
}
