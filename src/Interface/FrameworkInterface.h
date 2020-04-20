#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class FrameworkInterface
        : public ServantInterface
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool initializeFramework() = 0;
        virtual void finalizeFramework() = 0;

    public:
        virtual bool runFramework() = 0;
        virtual void stopFramework() = 0;

    public:
        virtual bool isAvailableFramework() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FrameworkInterface> FrameworkInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}