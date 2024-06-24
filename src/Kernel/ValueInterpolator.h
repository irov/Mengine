#pragma once

#include "Interface/EasingInterface.h"

#include "Kernel/UpdateContext.h"

#include "math/utils.h"

namespace Mengine
{
    template<typename T>
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

        void step( const EasingInterfacePtr & _easing, float _time, T * const _out )
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

        bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * const _out, float * const _used )
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
        virtual void _update( float _dt, T * const _out ) = 0;

    protected:
        bool m_started;
        float m_time;

        T m_value1;
        T m_value2;
        T m_prev;
        T m_delta;
        float m_duration;
    };
}
