#pragma once

#include "Kernel/ValueInterpolator.h"
#include "Kernel/BezierHelper.h"

namespace Mengine
{
    template<typename T, uint32_t N>
    class ValueInterpolatorBezier
        : public ValueInterpolator<T>
    {
    public:
        void setV( const T * _v )
        {
            for( uint32_t i = 0; i != N; ++i )
            {
                m_v[i] = _v[i];
            }
        }

    public:
        void start( const T & _value1, const T & _value2, const T * _v, float _duration )
        {
            ValueInterpolator<T>::start( _value1, _value2, _duration );

            this->setV( _v );
        }

        void _update( float _dt, T * const _out ) override
        {
            Helper::calculateBezierPosition<T, N>( _out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v, _dt );
        }

    protected:
        T m_v[N];
    };
}
