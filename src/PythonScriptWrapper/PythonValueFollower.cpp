#include "PythonValueFollower.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollower::PythonValueFollower()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollower::~PythonValueFollower()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollower::initialize( float _value, float _speed, const pybind::object & _cb, const pybind::args & _args )
    {
        this->resetValue( _value );
        this->setSpeed( _speed );

        m_cb = _cb;
        m_args = _args;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::setSpeed( float _value )
    {
        m_valueFollower.setSpeed( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollower::getSpeed() const
    {
        float value = m_valueFollower.getSpeed();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::setFollow( float _value )
    {
        m_valueFollower.setFollow( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollower::getFollow() const
    {
        float value = m_valueFollower.getFollow();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::setValue( float _value )
    {
        m_valueFollower.setValue( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonValueFollower::getValue() const
    {
        float value = m_valueFollower.getValue();

        return value;
    }
    //////////////////////////////////////////////////////////////////////////        
    void PythonValueFollower::resetValue( float _value )
    {
        m_valueFollower.reset( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonValueFollower::_affect( const UpdateContext * _context, float * _used )
    {
        m_valueFollower.update( _context, _used );

        float value = m_valueFollower.getValue();

        m_cb.call_args( value, m_args );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::complete()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::stop()
    {
        //Empty
    }

}