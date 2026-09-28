#pragma once

#include "Interface/ApplicationLifecycleInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ApplicationLifecycleable
        : public Mixin
    {
    public:
        ApplicationLifecycleable();
        ~ApplicationLifecycleable() override;

    public:
        virtual ApplicationLifecycleInterface * getApplicationLifecycleable();
        virtual const ApplicationLifecycleInterface * getApplicationLifecycleable() const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ApplicationLifecycleable> ApplicationLifecycleablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_APPLICATION_LIFECYCLEABLE()\
public:\
    Mengine::ApplicationLifecycleInterface * getApplicationLifecycleable() override { return this; }\
    const Mengine::ApplicationLifecycleInterface * getApplicationLifecycleable() const override { return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
