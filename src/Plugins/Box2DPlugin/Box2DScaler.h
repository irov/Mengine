#pragma once

#include "Box2DIncluder.h"

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
        MENGINE_INLINE float toBox2DWorld( float _v ) const
        {
            return _v * m_valueInv;
        }

        MENGINE_INLINE float toEngineWorld( float _v ) const
        {
            return _v * m_value;
        }

        MENGINE_INLINE b2Vec2 toBox2DWorld( const mt::vec2f & _v ) const
        {
            float x = this->toBox2DWorld( _v.x );
            float y = this->toBox2DWorld( _v.y );

            return {x, -y};
        }

        MENGINE_INLINE mt::vec2f toEngineWorld( const b2Vec2 & _v ) const
        {
            float x = this->toEngineWorld( _v.x );
            float y = this->toEngineWorld( _v.y );

            return mt::vec2f( x, -y );
        }

        MENGINE_INLINE b2Vec2 toBox2DWorldNormal( const mt::vec2f & _v ) const
        {
            return {_v.x, -_v.y};
        }

        MENGINE_INLINE mt::vec2f toEngineWorldNormal( const b2Vec2 & _v ) const
        {
            return mt::vec2f( _v.x, -_v.y );
        }

    protected:
        float m_value;
        float m_valueInv;
    };
}