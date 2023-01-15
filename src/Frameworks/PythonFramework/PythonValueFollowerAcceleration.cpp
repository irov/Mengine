#include "PythonValueFollowerAcceleration.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollowerAcceleration::PythonValueFollowerAcceleration()
        : m_cacheValue( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollowerAcceleration::~PythonValueFollowerAcceleration()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollowerAcceleration::initialize( float _value, float _speed, float _acceleration, const pybind::object & _cb, const pybind::args & _args )
    {
        this->resetValue( _value );
        this->setSpeed( _speed );
        this->setAcceleration( _acceleration );
        this->setCb( _cb, _args );

        m_cacheValue = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerAcceleration::setSpeed( float _value )
    {
        m_valueFollower.setSpeed( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerAcceleration::getSpeed() const
    {
        float value = m_valueFollower.getSpeed();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerAcceleration::setAcceleration( float _acceleration )
    {
        m_valueFollower.setAcceleration( _acceleration );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerAcceleration::getAcceleration() const
    {
        float acceleration = m_valueFollower.getAcceleration();

        return acceleration;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerAcceleration::setFollow( float _value )
    {
        m_valueFollower.setFollow( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerAcceleration::getFollow() const
    {
        float value = m_valueFollower.getFollow();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerAcceleration::setValue( float _value )
    {
        m_valueFollower.setValue( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerAcceleration::getValue() const
    {
        float value = m_valueFollower.getValue();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////        
    void PythonValueFollowerAcceleration::resetValue( float _value )
    {
        m_valueFollower.reset( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollowerAcceleration::_affect( const UpdateContext * _context, float * const _used )
    {
        m_valueFollower.update( _context, _used );

        float value = m_valueFollower.getValue();

        if( m_cacheValue != value )
        {
            m_cacheValue = value;

            this->callCb( value );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}