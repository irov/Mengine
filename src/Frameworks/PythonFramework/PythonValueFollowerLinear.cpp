#include "PythonValueFollowerLinear.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollowerLinear::PythonValueFollowerLinear()
        : m_cacheValue( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollowerLinear::~PythonValueFollowerLinear()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollowerLinear::initialize( float _value, float _speed, const pybind::object & _cb, const pybind::args & _args )
    {
        this->resetValue( _value );
        this->setSpeed( _speed );
        this->setCb( _cb, _args );

        m_cacheValue = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerLinear::setSpeed( float _value )
    {
        m_valueFollower.setSpeed( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerLinear::getSpeed() const
    {
        float value = m_valueFollower.getSpeed();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerLinear::setFollow( float _value )
    {
        m_valueFollower.setFollow( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerLinear::getFollow() const
    {
        float value = m_valueFollower.getFollow();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollowerLinear::setValue( float _value )
    {
        m_valueFollower.setValue( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollowerLinear::getValue() const
    {
        float value = m_valueFollower.getValue();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////        
    void PythonValueFollowerLinear::resetValue( float _value )
    {
        m_valueFollower.reset( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollowerLinear::_affect( const UpdateContext * _context, float * const _used )
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