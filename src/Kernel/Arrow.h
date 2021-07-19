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

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setArrowType( EArrowType _arrowType ) override;
        EArrowType getArrowType() const override;

    public:
        void setEntity( const EntityPtr & _entity ) override;
        const EntityPtr & getEntity() const override;

    public:
        void setClickPoint( const mt::vec2f & _clickPoint ) override;
        const mt::vec2f & getClickPoint() const override;

        void setClickPolygon( const Polygon & _clickPolygon ) override;
        const Polygon & getClickPolygon() const override;

        void setClickRadius( float _clickRadius ) override;
        float getClickRadius() const override;

    public:
        void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const override;
        void calcPointDeltha( const RenderContext * _context, const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha ) const override;

    public:
        void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const override;

    public:
        void adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const;
        void adaptScreenDeltha( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha ) const;
        void adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const;

    public:
        void onAppMouseLeave();
        void onAppMouseEnter();

    protected:
        EArrowType m_arrowType;

        EntityPtr m_entity;

        UniqueId m_inputMousePositionProviderId;

        mt::vec2f m_clickPoint;
        float m_clickRadius;
        Polygon m_clickPolygon;

        bool m_hided;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Arrow, ArrowInterface> ArrowPtr;
    //////////////////////////////////////////////////////////////////////////
}
