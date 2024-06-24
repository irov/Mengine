#pragma once

#include "Kernel/ValueInterpolator.h"
#include "Kernel/ParabolicHelper.h"

namespace Mengine
{
    template<typename T>
    class ValueInterpolatorParabolic
        : public ValueInterpolator<T>
    {
    public:
        void start( const T & _value1, const T & _value2, const T & _v0, float _duration )
        {
            ValueInterpolator<T>::start( _value1, _value2, _duration );

            m_v0 = _v0;
        }

        void step( float _timing, T * const _out )
        {
            if( _timing > ValueInterpolator<T>::m_duration )
            {
                *_out = ValueInterpolator<T>::m_value2;

                return;
            }

            float t_time = _timing / ValueInterpolator<T>::m_duration;

            Helper::calculateParabolicPosition( _out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v0, t_time );
        }

        void _update( float _dt, T * const _out ) override
        {
            Helper::calculateParabolicPosition( _out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v0, _dt );
        }

    protected:
        T m_v0;
    };
}
