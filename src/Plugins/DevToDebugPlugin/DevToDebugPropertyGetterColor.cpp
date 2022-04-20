#include "DevToDebugPropertyGetterColor.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterColor::DevToDebugPropertyGetterColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterColor::~DevToDebugPropertyGetterColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterColor::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterColor::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        Color value;
        m_getter( &value );

        if( _force == false )
        {
            if( m_test == value )
            {
                return false;
            }
        }

        m_test = value;

        ColorValue_ARGB argb = m_test.getAsARGB();

        Char hex_value[16] = {'\0'};
        MENGINE_SPRINTF( hex_value, "#%08X"
            , argb
        );

        _jwidget.set( _name.c_str(), hex_value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}