#include "TextArgumentValue.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextArgumentValue::TextArgumentValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextArgumentValue::~TextArgumentValue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TextArgumentValue::setValue( const String & _value )
    {
        m_value = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TextArgumentValue::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextArgumentValue::updateContext() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}