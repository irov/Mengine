#pragma once

#include "AppLovinInterstitialDelegate.h"
#include "AppLovinRewardedDelegate.h"
#include "AppLovinBannerDelegate.h"

//#include "Kernel/ServiceBase.h"
#include "Kernel/VectorConstString.h"


namespace Mengine
{
	class AppLovinService
//		: public ServiceBase<AppLovinServiceInterface>
	{
	public:
        AppLovinService();
		~AppLovinService() ;

    protected:
        bool _initializeService() ;
        void _finalizeService() ;
        
    public:
//        void setProvider( const AppleMARSDKProviderInterfacePtr & _provider ) override;
//        const AppleMARSDKProviderInterfacePtr & getProvider() const override;
        
    public:
        bool interstitialHasLoaded();
        bool showInterstitial();
        
        bool rewardedHasLoaded();
        bool showRewarded();
        
        void visibleBanner(bool show);
        
    public:
        void receivedReward(int amount);
        
	protected:
//        AppleMARSDKProviderInterfacePtr m_provider;
        
        AppLovinInterstitialDelegate * m_interstitialDelegate;
        AppLovinRewardedDelegate * m_rewardedDelegate;
        AppLovinBannerDelegate* m_bannerDelegate;
    };
}
