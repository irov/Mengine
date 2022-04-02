#include "DevToDebugWidgetText.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::~DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::setConstText( const String & _text )
    {
        m_text = _text;
        m_getter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::setGetterTitle( const LambdaGetterText & _getter )
    {
        m_getter = _getter;
    }
    ////////////////////////////////////////////////////////////////////////
    const String & DevToDebugWidgetText::calculateText() const
    {
        if( m_getter == nullptr )
        {
            return m_text;
        }

        m_getter( m_text );

        return m_text;
    }
    ////////////////////////////////////////////////////////////////////////
}