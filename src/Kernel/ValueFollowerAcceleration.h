#pragma once

#include "Kernel/ValueFollower.h"

namespace Mengine
{
    template<typename T>
    class ValueFollowerAcceleration
        : public ValueFollower<T>
    {
    public:
        ValueFollowerAcceleration()
            : m_speed( 0.f )
            , m_acceleration( 0.f )
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

        void setAcceleration( float _acceleration )
        {
            m_acceleration = _acceleration;
        }

        float getAcceleration() const
        {
            return m_acceleration;
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

            if( mt::equal_f_z( value_length ) == true )
            {
                this->overtake();

                return true;
            }

            m_speed += m_acceleration * _context->time;

            float step = m_speed * _context->time;

            if( step >= value_length )
            {
                this->overtake();

                return true;
            }
            else if( step + m_minimalDistance >= value_length )
            {
                T offset = this->getDistance();

                T add = offset * ((value_length - m_minimalDistance) / value_length);

                this->step( add );

                return true;
            }

            T offset = this->getDistance();

            T add = offset * (step / value_length);

            this->step( add );

            return false;
        }

    protected:
        float m_speed;
        float m_acceleration;
        float m_minimalDistance;
    };
}
