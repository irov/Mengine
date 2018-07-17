#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class UnityAdEventHandler
        : public Mixin
    {
    public:
        virtual void onUnityAdsReady( const String & _placementId ) = 0;
        virtual void onUnityAdsClick( const String & _placementId ) = 0;
        virtual void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) = 0;
        virtual void onUnityAdsStart( const String & _placementId ) = 0;
        virtual void onUnityAdsFinish( const String & _placementId, int _finishState ) = 0;
        virtual void onUnityAdsError( int _unityAdsError, const String & _message ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnityAdEventHandler> UnityAdEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
