#pragma once

#include "Interface/UnknownNodeInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Color.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GraphicsInterface
        : public UnknownNodeInterface
    {
    public:
        virtual void setLineWidth( float _width ) = 0;
        virtual float getLineWidth() const = 0;

        virtual void setOutlineWidth( float _outlineWidth ) = 0;
        virtual float getOutlineWidth() const = 0;

        virtual void setOutlineColor( const Color & _color ) = 0;
        virtual Color getOutlineColor() const = 0;

        virtual void setLineSoft( float _penumbra ) = 0;
        virtual float getLineSoft() const = 0;

        virtual void setLineColor( const Color & _color ) = 0;
        virtual Color getLineColor() const = 0;

        virtual void setCurveQuality( uint8_t _quality ) = 0;
        virtual uint8_t getCurveQuality() const = 0;

        virtual void setEllipseQuality( uint8_t _quality ) = 0;
        virtual uint8_t getEllipseQuality() const = 0;

        virtual void setRectQuality( uint8_t _quality ) = 0;
        virtual uint8_t getRectQuality() const = 0;

    public:
        virtual void pushState() = 0;
        virtual void popState() = 0;

    public:
        virtual void beginFill() = 0;
        virtual void endFill() = 0;

    public:
        virtual void pointMoveTo( const mt::vec2f & _point ) = 0;
        virtual void pointLineTo( const mt::vec2f & _point ) = 0;
        virtual void pointQuadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point ) = 0;
        virtual void pointBezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point ) = 0;

    public:
        virtual void drawRect( const mt::vec2f & _point, float _width, float _height ) = 0;
        virtual void drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius ) = 0;
        virtual void drawCircle( const mt::vec2f & _point, float _radius ) = 0;
        virtual void drawEllipse( const mt::vec2f & _point, float _width, float _height ) = 0;

    public:
        virtual void clear() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GraphicsInterface> GraphicsInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}