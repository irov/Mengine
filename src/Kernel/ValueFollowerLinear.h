#pragma once

#include "Kernel/ValueFollower.h"

namespace Mengine
{
    template<typename T>
    class ValueFollowerLinear
        : public ValueFollower<T>
    {
    public:
        ValueFollowerLinear()
            : m_speed( 0.f )
            , m_minimalDistance( 0.f )
        {
        }

    public:
        void setSpeed( float _speed )
        {
            m_speed = _speed;
        }

        float getSpeed() const
        {
            return m_speed;
        }

        void setMinimalDistance( float _minimalDistance )
        {
            m_minimalDistance = _minimalDistance;
        }

        float getMinimalDistance() const
        {
            return m_minimalDistance;
        }

    protected:
        bool _update( const UpdateContext * _context, float * const _used ) override
        {
            *_used = _context->time;

            float value_length = this->getLength();

            if( mt::equal_f_f( value_length, m_minimalDistance ) == true )
            {
                this->overtake();

                return false;
            }

            T offset = this->getDistance();

            float step = m_speed * _context->time;

            T add = (step + m_minimalDistance >= value_length) ? offset * (1.f - m_minimalDistance / value_length) : offset * (step / value_length);

            if( mt::sqrlength_f( add ) == 0.f )
            {
                return false;
            }

            this->step( add );

            return true;
        }

    protected:
        float m_speed;
        float m_minimalDistance;
    };
}
