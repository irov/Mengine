#pragma once

#include "Config/Config.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<uint32_t N>
        MENGINE_CONSTEXPR float calculateBezierFactorial()
        {
            if constexpr( N < 2 )
            {
                return 1.f;
            }
            else
            {
                return float( N ) * Helper::calculateBezierFactorial<N - 1>();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<uint32_t N>
        MENGINE_CONSTEXPR float calculateBezierIntegralPow( float _value )
        {
            if constexpr( N == 0 )
            {
                return 1.f;
            }
            else
            {
                return _value * Helper::calculateBezierIntegralPow<N - 1>( _value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N, uint32_t I>
        MENGINE_CONSTEXPR void calculateBezierPositionIteration( T * const _out, const T * _v, float _dt )
        {
            constexpr uint32_t degree = N + 1;
            constexpr float coefficient = Helper::calculateBezierFactorial<degree>() / (Helper::calculateBezierFactorial<I>() * Helper::calculateBezierFactorial<degree - I>());

            float t = Helper::calculateBezierIntegralPow<I>( _dt ) * Helper::calculateBezierIntegralPow<degree - I>( 1.f - _dt );

            const T & v = _v[I - 1];

            *_out += coefficient * t * v;

            if constexpr( I != N )
            {
                Helper::calculateBezierPositionIteration<T, N, I + 1>( _out, _v, _dt );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N>
        MENGINE_CONSTEXPR void calculateBezierPosition( T * const _out, const T & _begin, const T & _end, const T * _v, float _dt )
        {
            float t0 = Helper::calculateBezierIntegralPow<N + 1>( 1.f - _dt );
            float tn = Helper::calculateBezierIntegralPow<N + 1>( _dt );

            *_out = t0 * _begin + tn * _end;

            if constexpr( N != 0 )
            {
                Helper::calculateBezierPositionIteration<T, N, 1>( _out, _v, _dt );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N>
        MENGINE_CONSTEXPR void calculateBezierDerivative( T * const _out, const T & _begin, const T & _end, const T * _v, float _dt )
        {
            if constexpr( N == 0 )
            {
                *_out = _end - _begin;
            }
            else
            {
                constexpr float degree = float( N + 1 );

                T derivativePoints[N + 1];

                derivativePoints[0] = degree * (_v[0] - _begin);

                for( uint32_t i = 1; i != N; ++i )
                {
                    derivativePoints[i] = degree * (_v[i] - _v[i - 1]);
                }

                derivativePoints[N] = degree * (_end - _v[N - 1]);

                Helper::calculateBezierPosition<T, N - 1>( _out, derivativePoints[0], derivativePoints[N], derivativePoints + 1, _dt );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N, class F>
        MENGINE_CONSTEXPR float calculateBezierLength( const T & _begin, const T & _end, const T * _v, uint32_t _quality )
        {
            if( _quality < 2 )
            {
                _quality = 2;
            }

            if( (_quality & 1) != 0 )
            {
                ++_quality;
            }

            const float dt = 1.f / float( _quality );

            T derivativeBegin;
            Helper::calculateBezierDerivative<T, N>( &derivativeBegin, _begin, _end, _v, 0.f );

            T derivativeEnd;
            Helper::calculateBezierDerivative<T, N>( &derivativeEnd, _begin, _end, _v, 1.f );

            F length;

            float sum = length( derivativeBegin ) + length( derivativeEnd );

            const float doubleDt = 2.f * dt;

            float oddT = dt;

            for( uint32_t i = 1; i < _quality; i += 2, oddT += doubleDt )
            {
                T derivative;
                Helper::calculateBezierDerivative<T, N>( &derivative, _begin, _end, _v, oddT );

                sum += 4.f * length( derivative );
            }

            float evenT = doubleDt;

            for( uint32_t i = 2; i < _quality; i += 2, evenT += doubleDt )
            {
                T derivative;
                Helper::calculateBezierDerivative<T, N>( &derivative, _begin, _end, _v, evenT );

                sum += 2.f * length( derivative );
            }

            return sum * dt / 3.f;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
