#pragma once

#include "Interface/EasingInterface.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"

#include "math/utils.h"

namespace Mengine
{
    template <typename T>
    class ValueAccumulator
    {
    public:
        ValueAccumulator()
            : m_started( false )
            , m_time( 0.f )
        {
        }

        ~ValueAccumulator()
        {
        }

    public:
        void stop()
        {
            m_started = false;
        }

        bool isStarted() const
        {
            return m_started;
        }

        virtual bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * _out, float * _used ) = 0;

    protected:
        bool m_started;
        float m_time;

        T m_pos;
        T m_delta;
    };

    template <typename T>
    class ValueAccumulateLinear
        : public ValueAccumulator<T>
    {
    public:
        void start( const T & _pos, const T & _acc, float _speed )
        {
            MENGINE_ASSERTION( _speed >= 0.f );

            ValueAccumulator<T>::m_started = false;

            ValueAccumulator<T>::m_pos = _pos;
            ValueAccumulator<T>::m_delta = _acc * _speed;
            ValueAccumulator<T>::m_time = 0.f;

            ValueAccumulator<T>::m_started = true;
        }

        bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * _out, float * _used ) override
        {
            MENGINE_UNUSED( _easing );

            *_used = _context->time;

            if( ValueAccumulator<T>::m_started == false )
            {
                *_out = ValueAccumulator<T>::m_pos;

                return true;
            }

            ValueAccumulator<T>::m_time += _context->time;

            ValueAccumulator<T>::m_pos += ValueAccumulator<T>::m_delta * _context->time;

            *_out = ValueAccumulator<T>::m_pos;

            return false;
        }
    };

    template <typename T>
    class ValueInterpolator
    {
    public:
        ValueInterpolator()
            : m_started( false )
            , m_time( 0.f )
            , m_duration( 0.f )
        {
        }

    public:
        void setValue1( const T & _value )
        {
            m_value1 = _value;
        }

        void setValue2( const T & _value )
        {
            m_value2 = _value;
        }

    public:
        void start( const T & _value1, const T & _value2, float _duration )
        {
            m_value1 = _value1;
            m_value2 = _value2;
            m_prev = m_value1;
            m_delta = m_prev - m_value1;
            m_duration = _duration;
            m_time = 0.f;
            m_started = true;
        }

        void stop()
        {
            m_started = false;
        }

        bool isStarted() const
        {
            return m_started;
        }

        const T & getValue() const
        {
            return m_prev;
        }

        void step( const EasingInterfacePtr & _easing, float _time, T * _out )
        {
            if( _time > m_duration )
            {
                *_out = m_value2;

                return;
            }

            float dt = m_time / m_duration;

            if( _easing != nullptr )
            {
                float easing_dt = _easing->easing( dt );

                this->_update( easing_dt, _out );
            }
            else
            {
                this->_update( dt, _out );
            }
        }

        bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * _out, float * _used )
        {
            if( m_started == false )
            {
                *_out = m_value2;
                *_used = 0.f;

                return true;
            }

            if( m_duration < mt::constant::eps )
            {
                *_used = 0.f;

                m_duration = 0.f;
                m_time = 0.f;
                *_out = m_value2;
                m_delta = m_value2 - m_value1;
                m_started = false;

                return true;
            }

            if( m_time + _context->time >= m_duration )
            {
                *_used = m_duration - m_time;

                m_duration = 0.f;
                m_time = 0.f;
                *_out = m_value2;
                m_delta = m_value2 - m_prev;
                m_started = false;

                return true;
            }

            m_time += _context->time;

            float dt = m_time / m_duration;

            if( _easing != nullptr )
            {
                float easing_dt = _easing->easing( dt );

                this->_update( easing_dt, _out );
            }
            else
            {
                this->_update( dt, _out );
            }

            m_delta = (*_out) - m_prev;
            m_prev = (*_out);

            *_used = _context->time;

            return false;
        }

    protected:
        virtual void _update( float _dt, T * _out ) = 0;

    protected:
        bool m_started;
        float m_time;

        T m_value1;
        T m_value2;
        T m_prev;
        T m_delta;
        float m_duration;
    };

    template <typename T>
    class ValueInterpolatorLinear
        : public ValueInterpolator<T>
    {
    public:
        void _update( float _dt, T * _out ) override
        {
            *_out = ValueInterpolator<T>::m_value1 + (ValueInterpolator<T>::m_value2 - ValueInterpolator<T>::m_value1) * _dt;
        }
    };

    template <typename T>
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

        void _update( float _dt, T * _out ) override
        {
            MENGINE_UNUSED( _dt );

            *_out = ValueInterpolator<T>::m_value1 + m_v0 * ValueInterpolator<T>::m_time + m_a * 0.5f * ValueInterpolator<T>::m_time * ValueInterpolator<T>::m_time;
        }

    protected:
        T m_v0;
        T m_a;
    };
    //////////////////////////////////////////////////////////////////////////
    template <typename T, uint32_t N>
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

        void _update( float _dt, T * _out ) override
        {
            Helper::calculateBezierPosition( *_out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v, _dt );
        }

    protected:
        T m_v[N];
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void calculateParabolicPosition( T & _out, const T & _begin, const T & _end, const T & _height, float _time )
    {
        T a = 2.f * _end - 4.f * _height + 2.f * _begin;
        T b = 4.f * _height - _end - 3.f * _begin;
        T c = _begin;

        _out = a * _time * _time + b * _time + c;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T, class LENGTH>
    float calculateParabolicLength( const T & _begin, const T & _end, const T & _height, LENGTH _length )
    {
        const uint32_t max_iterator = 10;

        float total_length = 0.f;

        T prevPoint = _begin;

        const float dt = 1.f / float( max_iterator );

        float t = 0.f;

        for( uint32_t i = 1; i != max_iterator; ++i )
        {
            t += dt;

            T nextPoint;
            calculateParabolicPosition( nextPoint, _begin, _end, _height, t );

            float length = _length( nextPoint, prevPoint );

            total_length += length;

            prevPoint = nextPoint;
        }

        float length = _length( _end, prevPoint );

        total_length += length;

        return total_length;
    }
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
    class ValueInterpolatorParabolic
        : public ValueInterpolator<T>
    {
    public:
        void start( const T & _value1, const T & _value2, const T & _v0, float _duration )
        {
            ValueInterpolator<T>::start( _value1, _value2, _duration );

            m_v0 = _v0;
        }

        void step( float _timing, T * _out )
        {
            if( _timing > ValueInterpolator<T>::m_duration )
            {
                *_out = ValueInterpolator<T>::m_value2;

                return;
            }

            float t_time = _timing / ValueInterpolator<T>::m_duration;

            calculateParabolicPosition( *_out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v0, t_time );
        }

        void _update( float _dt, T * _out ) override
        {
            calculateParabolicPosition( *_out, ValueInterpolator<T>::m_value1, ValueInterpolator<T>::m_value2, m_v0, _dt );
        }

    protected:
        T m_v0;
    };
}
