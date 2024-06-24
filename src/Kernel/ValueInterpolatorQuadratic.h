#pragma once

#include "Kernel/ValueInterpolator.h"

namespace Mengine
{
    template<typename T>
    class ValueInterpolatorQuadratic
        : public ValueInterpolator<T>
    {
    public:
        void start( const T & _value1, const T & _value2, const T & _v0, float _duration )
        {
            ValueInterpolator<T>::start( _value1, _value2, _duration );

            m_v0 = _v0;

            if( _duration > mt::constant::eps )
            {
                float invTime = 1.f / ValueInterpolator<T>::m_duration;
                m_a = (ValueInterpolator<T>::m_value2 - ValueInterpolator<T>::m_value1 - m_v0 * ValueInterpolator<T>::m_duration) * 2.f * invTime * invTime;
            }
        }

        void _update( float _dt, T * const _out ) override
        {
            MENGINE_UNUSED( _dt );

            *_out = ValueInterpolator<T>::m_value1 + m_v0 * ValueInterpolator<T>::m_time + m_a * 0.5f * ValueInterpolator<T>::m_time * ValueInterpolator<T>::m_time;
        }

    protected:
        T m_v0;
        T m_a;
    };
}
