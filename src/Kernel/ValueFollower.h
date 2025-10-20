#pragma once

#include "Interface/UpdationInterface.h"

#include "math/utils.h"

namespace Mengine
{
    template<typename T>
    class ValueFollower
    {
    public:
        ValueFollower()
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
            float length = mt::length_f( m_follow, m_value );

            return length;
        }

        void setFollow( const T & _follow )
        {
            m_follow = _follow;
        }

        bool update( const UpdateContext * _context, float * const _used )
        {
            bool process = this->_update( _context, _used );

            return process;
        }

    protected:
        virtual bool _update( const UpdateContext * _context, float * const _used ) = 0;

    protected:
        T m_value;
        T m_follow;
    };
}
