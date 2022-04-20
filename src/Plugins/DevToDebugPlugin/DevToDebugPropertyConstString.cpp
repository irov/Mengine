#include "DevToDebugPropertyConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstString::DevToDebugPropertyConstString()
        : m_invalidate( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstString::~DevToDebugPropertyConstString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyConstString::setValue( const String & _value )
    {
        if( m_value == _value )
        {
            return;
        }

        m_value = _value;

        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & DevToDebugPropertyConstString::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyConstString::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        if( _force == false )
        {
            if( m_invalidate == false )
            {
                return false;
            }
        }

        _jwidget.set( _name.c_str(), m_value );

        m_invalidate = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}