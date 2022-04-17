#include "DevToDebugPropertyConstBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstBoolean::DevToDebugPropertyConstBoolean()
        : m_invalidate( true )
        , m_value( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstBoolean::~DevToDebugPropertyConstBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyConstBoolean::setValue( bool _value )
    {
        if( m_value == _value )
        {
            return;
        }

        m_value = _value;

        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyConstBoolean::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyConstBoolean::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
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