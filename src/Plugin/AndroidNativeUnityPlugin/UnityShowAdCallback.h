#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class UnityShowAdCallback
        : public Mixin
    {
    public:
        virtual void onUnityAdsClick( const String & _placementId ) = 0;
        virtual void onUnityAdsPlacementStateChanged( const String & _placementId, const int _placementState, const int _placementState1 ) = 0;
        virtual void onUnityAdsStart( const String & _placementId ) = 0;
        virtual void onUnityAdsFinish( const String & _placementId, const int _finishState ) = 0;
        virtual void onUnityAdsError( const int _unityAdsError, const String & _message ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnityShowAdCallback> UnityShowAdCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
