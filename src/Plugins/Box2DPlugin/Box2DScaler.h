#pragma once

#include "Box2D/Box2D.h"

namespace Mengine
{
    class Box2DScaler
    {
    public:
        MENGINE_INLINE Box2DScaler()
            : m_value( 0.f )
            , m_valueInv( 0.f )
        {
        }

        MENGINE_INLINE explicit Box2DScaler( float _value )
            : m_value( _value )
            , m_valueInv( 1.f / _value )
        {
        }

        MENGINE_INLINE ~Box2DScaler()
        {
        }

    public:
        MENGINE_INLINE float32 toBox2DWorld( float _v ) const
        {
            return (float32)_v * m_valueInv;
        }

        MENGINE_INLINE float toEngineWorld( float32 _v ) const
        {
            return (float32)_v * m_value;
        }

        MENGINE_INLINE b2Vec2 toBox2DWorld( const mt::vec2f & _v ) const
        {
            float32 x = this->toBox2DWorld( _v.x );
            float32 y = this->toBox2DWorld( _v.y );

            return b2Vec2( x, y );
        }

        MENGINE_INLINE mt::vec2f toEngineWorld( const b2Vec2 & _v ) const
        {
            float x = this->toEngineWorld( _v.x );
            float y = this->toEngineWorld( _v.y );

            return mt::vec2f( x, y );
        }

    protected:
        float m_value;
        float m_valueInv;
    };
}