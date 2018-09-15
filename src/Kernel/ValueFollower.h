#pragma once

#include "Interface/UpdationInterface.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
    class ValueFollower
    {
    public:
        ValueFollower() noexcept
            : m_value( T() )
            , m_follow( T() )
        {
        }

    public:
        void initialize( const T & _value )
        {
            m_value = _value;
            m_follow = _value;
        }

        void setValue( const T & _value )
        {
            m_value = _value;
        }

        const T & getValue() const
        {
            return m_value;
        }

        const T & getFollow() const
        {
            return m_follow;
        }

        T getDistance() const
        {
            T distance = m_follow - m_value;

            return distance;
        }

        void reset( const T & _value )
        {
            m_value = _value;
            m_follow = _value;
        }

        void overtake()
        {
            m_value = m_follow;
        }

        void step( const T & _value )
        {
            m_value += _value;
        }

        float getLength() const
        {
            float length = mt::length( m_follow, m_value );

            return length;
        }

        void setFollow( const T & _follow )
        {
            m_follow = _follow;
        }

        bool update( const UpdateContext * _context, float * _used )
        {
            bool successful = this->_update( _context, _used );

            return successful;
        }

    protected:
        virtual bool _update( const UpdateContext * _context, float * _used ) = 0;

    protected:
        T m_value;
        T m_follow;
    };
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
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
        bool _update( const UpdateContext * _context, float * _used ) override
        {
            *_used = _context->time;

            float value_length = this->getLength();

            if( mt::equal_f_z( value_length ) == true )
            {
                this->overtake();

                return true;
            }

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
        float m_minimalDistance;
    };
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
    class ValueFollowerAcceleration
        : public ValueFollower<T>
    {
    public:
        ValueFollowerAcceleration()
            : m_speed( 0.f )
            , m_acceleration( 0.f )
            , m_distance( 0.f )
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

        void setDistance( float _distance )
        {
            m_distance = _distance;
        }

        float getDistance() const
        {
            return m_distance;
        }

    protected:
        bool _update( float _current, float _timing ) override
        {
            (void)_current;

            float value_length = this->getLength();

            if( mt::equal_f_z( value_length ) == true )
            {
                this->overtake();

                return true;
            }

            m_speed += m_acceleration * _timing;

            float step = m_speed * _timing;

            if( step >= value_length )
            {
                this->overtake();

                return true;
            }
            else if( step + m_distance >= value_length )
            {
                T offset = this->getDistance();

                T add = offset * ((value_length - m_distance) / value_length);

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
        float m_distance;
    };
}
