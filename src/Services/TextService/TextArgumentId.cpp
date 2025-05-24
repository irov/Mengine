#include "TextArgumentId.h"

#include "Interface/TextServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextArgumentId::TextArgumentId()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextArgumentId::~TextArgumentId()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TextArgumentId::setTextId( const ConstString & _textId )
    {
        m_textId = _textId;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextArgumentId::getTextId() const
    {
        return m_textId;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TextArgumentId::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextArgumentId::updateContext() const
    {
        const TextEntryInterfacePtr & textEntry = TEXT_SERVICE()
            ->getTextEntry( m_textId );

        const String & value = textEntry->getValue();

        if( m_value == value )
        {
            return false;
        }

        m_value = value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}