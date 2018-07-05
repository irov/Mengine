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
        virtual void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) = 0;
        virtual void onUnityAdsStart( const String & _placementId ) = 0;
        virtual void onUnityAdsFinish( const String & _placementId, int _finishState ) = 0;
        virtual void onUnityAdsError( int _unityAdsError, const String & _message ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnityShowAdCallback> UnityShowAdCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
