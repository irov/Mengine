#pragma once

#include "ArrowInputMousePositionProvider.h"

#include "Kernel/Entity.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Resolution.h"
#include "Kernel/Polygon.h"

#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EArrowType
    {
        EAT_POINT,
        EAT_RADIUS,
        EAT_POLYGON
    };
    //////////////////////////////////////////////////////////////////////////
    class Arrow
        : public Entity
    {
    public:
        Arrow();
        ~Arrow() override;

    public:
        EArrowType getArrowType() const;

    public:
        void setOffsetClick( const mt::vec2f & _offsetClick );
        const mt::vec2f & getOffsetClick() const;

        void setPolygon( const Polygon & _polygon );
        const Polygon & getPolygon() const;

        void setRadius( float _radius );
        float getRadius() const;

    public:
        void calcMouseWorldPosition( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const;
        void calcPointClick( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const;
        void calcPointDeltha( const RenderCameraInterfacePtr & _camera, const mt::vec2f & _screenDeltha, mt::vec2f * _worldDeltha ) const;

    public:
        void calcMouseScreenPosition( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const;

    public:
        void adaptScreenPosition_( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const;
        void adaptScreenDeltha_( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha ) const;
        void adaptWorldPosition_( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const;

    public:
        void onAppMouseLeave();
        void onAppMouseEnter();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        EArrowType m_arrowType;

        uint32_t m_inputMousePositionProviderId;

        mt::vec2f m_pointClick;
        float m_radius;
        Polygon m_polygon;
        mutable Polygon m_polygonScreen;

        bool m_hided;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Arrow> ArrowPtr;
    //////////////////////////////////////////////////////////////////////////
}
