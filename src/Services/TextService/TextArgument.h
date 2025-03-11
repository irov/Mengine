#pragma once

#include "Interface/TextArgumentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextArgument
        : public TextArgumentInterface
    {
        DECLARE_FACTORABLE( TextArgument );

    public:
        TextArgument();
        ~TextArgument() override;

    public:
        void setValue( const String & _value ) override;
        const String & getValue() const override;

        void setContext( const LambdaTextArgumentContext & _context ) override;
        const LambdaTextArgumentContext & getContext() const override;

    public:
        bool updateContext() const override;

    protected:
        mutable String m_value;

        LambdaTextArgumentContext m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgument, TextArgumentInterface> TextArgumentPtr;
    //////////////////////////////////////////////////////////////////////////
}