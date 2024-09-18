#pragma once

#include "Interface/ArrowServiceInterface.h"
#include "Interface/InputMousePositionProviderInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArrowService
        : public ServiceBase<ArrowServiceInterface>
        , public InputMousePositionProviderInterface
    {
    public:
        ArrowService();
        ~ArrowService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

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
        const NodePtr & getNode() const override;

    protected:
        void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) const override;

    protected:
        void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const override;

    protected:
        void onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure ) override;

    protected:
        void notifyInitializeComplete_();

    protected:
        EArrowType m_arrowType;

        mt::vec2f m_pointClick;
        float m_radius;
        Polygon m_polygon;
        mutable Polygon m_polygonScreen;

        NodePtr m_node;

        bool m_hided;
    };
}
