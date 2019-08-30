#pragma once

#include "Interface/Interface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class FrameworkInterface
        : public Interface
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool initializeFramework() = 0;
        virtual void finalizeFramework() = 0;

    public:
        virtual bool isAvailableFramework() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FrameworkInterface> FrameworkInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}