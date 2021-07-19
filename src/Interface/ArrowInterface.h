#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/RenderContext.h"
#include "Kernel/Resolution.h"
#include "Kernel/Polygon.h"

#include "math/vec2.h"

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
    typedef IntrusivePtr<class Entity, class Node> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
    class ArrowInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setArrowType( EArrowType _arrowType ) = 0;
        virtual EArrowType getArrowType() const = 0;

    public:
        virtual void setEntity( const EntityPtr & _entity ) = 0;
        virtual const EntityPtr & getEntity() const = 0;

    public:
        virtual void setClickPoint( const mt::vec2f & _clickPoint ) = 0;
        virtual const mt::vec2f & getClickPoint() const = 0;

        virtual void setClickPolygon( const Polygon & _clickPolygon ) = 0;
        virtual const Polygon & getClickPolygon() const = 0;

        virtual void setClickRadius( float _clickRadius ) = 0;
        virtual float getClickRadius() const = 0;

    public:
        virtual void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const = 0;
        virtual void calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const = 0;
        virtual void calcPointDeltha( const RenderContext * _context, const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha ) const = 0;

    public:
        virtual void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const = 0;

    public:
        virtual void adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const = 0;
        virtual void adaptScreenDeltha( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha ) const = 0;
        virtual void adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArrowInterface> ArrowInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}