#pragma once

#include "Interface/TextArgumentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextArgumentValue
        : public TextArgumentInterface
    {
        DECLARE_FACTORABLE( TextArgumentValue );

    public:
        TextArgumentValue();
        ~TextArgumentValue() override;

    public:
        void setValue( const String & _value );

    public:
        const String & getValue() const override;

    public:
        bool updateContext() const override;

    protected:
        String m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgumentValue, TextArgumentInterface> TextArgumentValuePtr;
    //////////////////////////////////////////////////////////////////////////
}