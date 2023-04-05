#include "DevToDebugPropertyConstColor.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstColor::DevToDebugPropertyConstColor()
        : m_invalidate( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyConstColor::~DevToDebugPropertyConstColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyConstColor::setValue( const Color & _value )
    {
        if( m_value == _value )
        {
            return;
        }

        m_value = _value;

        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & DevToDebugPropertyConstColor::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyConstColor::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        if( _force == false )
        {
            if( m_invalidate == false )
            {
                return false;
            }
        }

        ColorValue_ARGB argb = m_value.getAsARGB();

        argb &= 0x00FFFFFF;

        Char hex_value[32] = {'\0'};
        MENGINE_SNPRINTF( hex_value, 32, "#%06X"
            , argb
        );

        _jwidget.set( _name.c_str(), hex_value );

        m_invalidate = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
