#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class UnitySetupCallback
        : public Mixin
    {
    public:
        virtual void onUnityAdsReady( const String & _placementId ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnitySetupCallback> UnitySetupCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
