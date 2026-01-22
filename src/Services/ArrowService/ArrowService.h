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
        DECLARE_FACTORABLE( ArrowService );

    public:
        ArrowService();
        ~ArrowService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        EArrowType getArrowType() const override;

    protected:
        void setArrowTypePoint() override;

        void setArrowTypePolygon( const Polygon & _polygon ) override;
        const Polygon & getArrowTypePolygon() const override;

        void setArrowTypeRadius( float _radius ) override;
        float getArrowTypeRadius() const override;

    protected:
        const NodePtr & getArrowNode() const override;

    protected:
        void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcMouseWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) const override;

    protected:
        void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const override;

    protected:
        void onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure ) override;

    protected:
        void notifyInitializeComplete_();

    protected:
        EArrowType m_arrowType;

        float m_arrowRadius;
        Polygon m_arrowPolygon;
        mutable Polygon m_arrowPolygonScreen;

        NodePtr m_arrowNode;

        bool m_hided;
    };
}
