#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class FacebookInitializationCallback
        : public Mixin
    {
    public:
        virtual void onSDKInitialized() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookInitializationCallback> FacebookInitializationCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
