#	pragma once

#	include "Utils/Core/Polygon.h"
#	include "Box2D/Box2D.h"

namespace Menge
{
    class Box2DPhysicScaler
    {
    public:
        Box2DPhysicScaler( );
        virtual ~Box2DPhysicScaler();

    public:
        float scaleToBox2D( float _value ) const;
        float scaleToMenge( float _value ) const;

        void scaleVectorToMenge( mt::vec2f& _point ) const;
        void scalePositionToMenge( mt::vec2f& _point ) const;
        void scaleVectorToMenge( b2Vec2& _point ) const;
        void scalePositionToMenge( b2Vec2& _point ) const;

        void scaleVectorToBox2D( mt::vec2f& _point ) const;
        void scalePositionToBox2D( mt::vec2f& _point ) const;
        void scaleVectorToBox2D( b2Vec2& _point ) const;
        void scalePositionToBox2D( b2Vec2& _point ) const;
        float convertX( float _x ) const;
        float convertY( float _y ) const;
        void setLimits( const mt::vec2f& _limits );
        const mt::vec2f& getLimits() const;

    protected:
        mt::vec2f m_limits;
    };
}