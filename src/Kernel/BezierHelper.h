#pragma once

#include "Config/Config.h"

#include "math/factorial.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N>
        MENGINE_CONSTEXPR void calculateBezierPosition( T * const _out, const T & _begin, const T & _end, const T * _v, float _dt )
        {
            uint32_t n = N + 1;

            float t0 = mt::integral_powf( 1.f - _dt, n );
            float tn = mt::integral_powf( _dt, n );

            *_out = t0 * _begin + tn * _end;

            float f_count = mt::factorialf( n );

            for( uint32_t i = 1; i != n; ++i )
            {
                float c = f_count / (mt::factorialf( i ) * mt::factorialf( n - i ));
                float t = mt::integral_powf( _dt, i ) * mt::integral_powf( 1.f - _dt, n - i );

                const T & v = _v[i - 1];

                *_out += c * t * v;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N, class F>
        MENGINE_CONSTEXPR float calculateBezierLength( const T & _begin, const T & _end, const T * _v, uint32_t _quality, F _length )
        {
            float total_length = 0.f;

            T prevPoint = _begin;

            const float dt = 1.f / float( _quality - 1 );

            float t = 0.f;

            for( uint32_t i = 1; i != _quality; ++i )
            {
                t += dt;

                T nextPoint;
                Helper::calculateBezierPosition<T, N>( nextPoint, _begin, _end, _v, t );

                float length = _length( nextPoint, prevPoint );

                total_length += length;

                prevPoint = nextPoint;
            }

            return total_length;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}