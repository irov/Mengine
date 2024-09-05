#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Polygon.h"
#include "Kernel/RenderContext.h"

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
    class ArrowInterface
        : public ServantInterface
    {
    public:
        virtual EArrowType getArrowType() const = 0;

    public:
        virtual void setOffsetClick( const mt::vec2f & _offsetClick ) = 0;
        virtual const mt::vec2f & getOffsetClick() const = 0;

        virtual void setPolygon( const Polygon & _polygon ) = 0;
        virtual const Polygon & getPolygon() const = 0;

        virtual void setRadius( float _radius ) = 0;
        virtual float getRadius() const = 0;

    public:
        virtual void setNode( const NodePtr & _node ) = 0;
        virtual const NodePtr & getNode() const = 0;

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const = 0;
        virtual void calcPointClick( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const = 0;
        virtual void calcPointDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * _worldDelta ) const = 0;

    public:
        virtual void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArrowInterface> ArrowInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}