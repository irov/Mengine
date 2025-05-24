#pragma once

#include "Interface/TextArgumentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextArgumentId
        : public TextArgumentInterface
    {
        DECLARE_FACTORABLE( TextArgumentId );

    public:
        TextArgumentId();
        ~TextArgumentId() override;

    public:
        void setTextId( const ConstString & _textId );
        const ConstString & getTextId() const;

    public:
        const String & getValue() const override;

    public:
        bool updateContext() const override;

    protected:
        mutable String m_value;

        ConstString m_textId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgumentId, TextArgumentInterface> TextArgumentIdPtr;
    //////////////////////////////////////////////////////////////////////////
}