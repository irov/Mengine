#pragma once

#include "Interface/ArrowInterface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Arrow
        : public ArrowInterface
    {
        DECLARE_FACTORABLE( Arrow );

    public:
        Arrow();
        ~Arrow() override;

    protected:
        EArrowType getArrowType() const override;

    protected:
        void setOffsetClick( const mt::vec2f & _offsetClick ) override;
        const mt::vec2f & getOffsetClick() const override;

        void setPolygon( const Polygon & _polygon ) override;
        const Polygon & getPolygon() const override;

        void setRadius( float _radius ) override;
        float getRadius() const override;

    protected:
        void setNode( const NodePtr & _node ) override;
        const NodePtr & getNode() const override;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * _worldDelta ) const override;

    protected:
        void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const override;

    protected:
        EArrowType m_arrowType;

        UniqueId m_inputMousePositionProviderId;

        mt::vec2f m_pointClick;
        float m_radius;
        Polygon m_polygon;
        mutable Polygon m_polygonScreen;

        NodePtr m_node;

        bool m_hided;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Arrow, ArrowInterface> ArrowPtr;
    //////////////////////////////////////////////////////////////////////////
}
