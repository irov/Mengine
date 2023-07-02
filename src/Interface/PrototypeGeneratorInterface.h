#pragma once

#include "Interface/Interface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrototypeGeneratorInterface
        : public Interface
    {
    public:
        virtual void setCategory( const ConstString & _category ) = 0;
        virtual const ConstString & getCategory() const = 0;

        virtual void setPrototype( const ConstString & _prototype ) = 0;
        virtual const ConstString & getPrototype() const = 0;

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual FactorablePointer generate( const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual uint32_t count() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrototypeGeneratorInterface> PrototypeGeneratorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
