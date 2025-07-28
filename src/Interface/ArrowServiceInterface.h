#pragma once

#include "Interface/ServiceInterface.h"

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
    class ArrowServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ArrowService" );

    public:
        virtual EArrowType getArrowType() const = 0;

    public:
        virtual void setArrowPoint() = 0;

        virtual void setArrowPolygon( const Polygon & _polygon ) = 0;
        virtual const Polygon & getArrowPolygon() const = 0;

        virtual void setArrowRadius( float _radius ) = 0;
        virtual float getArrowRadius() const = 0;

    public:
        virtual const NodePtr & getArrowNode() const = 0;

    public:
        virtual void calcMouseWorldPosition( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) const = 0;
        virtual void calcMouseWorldDelta( const RenderContext * _context, const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) const = 0;

    public:
        virtual void calcMouseScreenPosition( const RenderContext * _context, const mt::vec2f & _worldPoint, mt::vec2f * const _screenPoint ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define ARROW_SERVICE()\
    ((Mengine::ArrowServiceInterface *)SERVICE_GET(Mengine::ArrowServiceInterface))
//////////////////////////////////////////////////////////////////////////