#include "DevToDebugPropertyInitialBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyInitialBoolean::DevToDebugPropertyInitialBoolean()
        : m_value( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyInitialBoolean::~DevToDebugPropertyInitialBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyInitialBoolean::setValue( bool _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyInitialBoolean::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyInitialBoolean::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        if( _force == false )
        {
            return false;
        }

        _jwidget.set( _name.c_str(), m_value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}