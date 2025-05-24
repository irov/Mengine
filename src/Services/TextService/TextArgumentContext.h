#pragma once

#include "Interface/TextArgumentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextArgumentContext
        : public TextArgumentInterface
    {
        DECLARE_FACTORABLE( TextArgumentContext );

    public:
        TextArgumentContext();
        ~TextArgumentContext() override;

    public:
        void setContext( const LambdaTextArgumentContext & _context );
        const LambdaTextArgumentContext & getContext() const;

    public:
        const String & getValue() const override;

    public:
        bool updateContext() const override;

    protected:
        mutable String m_value;

        LambdaTextArgumentContext m_context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgumentContext, TextArgumentInterface> TextArgumentContextPtr;
    //////////////////////////////////////////////////////////////////////////
}