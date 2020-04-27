#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EnumUnityAdsEventHandler
    {
        UNITYADS_INITIALIZE,
        UNITYADS_READY,
        UNITYADS_CLICK,
        UNITYADS_PLACEMENT_STATE_CHANGED,
        UNITYADS_START,
        UNITYADS_FINISH,
        UNITYADS_ERROR,
    };
    //////////////////////////////////////////////////////////////////////////
    class UnityAdsEventHandler
        : public Mixin
    {
    public:
        virtual void onUnityAdsInitialized() = 0;

    public:
        virtual void onUnityAdsReady( const String & _placementId ) = 0;
        virtual void onUnityAdsClick( const String & _placementId ) = 0;
        virtual void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) = 0;
        virtual void onUnityAdsStart( const String & _placementId ) = 0;
        virtual void onUnityAdsFinish( const String & _placementId, int _finishState ) = 0;
        virtual void onUnityAdsError( int _unityAdsError, const String & _message ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnityAdsEventHandler> UnityAdsEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
