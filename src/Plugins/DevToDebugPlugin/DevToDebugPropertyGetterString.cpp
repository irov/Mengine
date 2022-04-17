#include "DevToDebugPropertyGetterString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterString::DevToDebugPropertyGetterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterString::~DevToDebugPropertyGetterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterString::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterString::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        String value;
        m_getter( &value );

        if( _force == false )
        {
            if( m_test == value )
            {
                return false;
            }
        }

        m_test = value;

        _jwidget.set( _name.c_str(), m_test );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}