#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( String * )> LambdaTextArgumentContext;
    //////////////////////////////////////////////////////////////////////////
    class TextArgumentInterface
        : public ServantInterface
    {
    public:
        virtual void setValue( const String & _value ) = 0;
        virtual const String & getValue() const = 0;
        
        virtual void setContext( const LambdaTextArgumentContext & _context ) = 0;
        virtual const LambdaTextArgumentContext & getContext() const = 0;

    public:
        virtual bool updateContext() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextArgumentInterface> TextArgumentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
