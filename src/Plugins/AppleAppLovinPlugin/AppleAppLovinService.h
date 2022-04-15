#pragma once

#include "AppLovinInterstitialDelegate.h"
#include "AppLovinRewardedDelegate.h"
#include "AppLovinBannerDelegate.h"
#include "AppLovinInterface.h"

//#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"


namespace Mengine
{
	class AppleAppLovinService
        : public Mengine::AppLovinRewardCallback
		: public ServiceBase<AppleAppLovinServiceInterface>
	{
	public:
        AppleAppLovinService();
		~AppleAppLovinService() ;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        bool interstitialHasLoaded();
        bool showInterstitial();
        
        bool rewardedHasLoaded();
        bool showRewarded();        
        
    public:
        void receivedReward( uint64_t amount );

    public:
        void showBanner();
        void hideBanner();
        
	protected:       
        AppLovinInterstitialDelegate * m_interstitialDelegate;
        AppLovinRewardedDelegate * m_rewardedDelegate;
        AppLovinBannerDelegate* m_bannerDelegate;
    };
}
