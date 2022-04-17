#include "DevToDebugPropertyGetterBoolean.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterBoolean::DevToDebugPropertyGetterBoolean()
        : m_test( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterBoolean::~DevToDebugPropertyGetterBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterBoolean::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterBoolean::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        bool value;
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