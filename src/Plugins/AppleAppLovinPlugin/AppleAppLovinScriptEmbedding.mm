#include "AppleAppLovinScriptEmbedding.h"

#include "AppleAppLovinInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinBannerProvider
            : public PythonCallbackProvider<AppleAppLovinBannerProviderInterface>
        {
        public:
            PythonAppleAppLovinBannerProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleAppLovinBannerProviderInterface>( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinBannerDidLoadAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidLoadAd" );
            }
            
            void onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier" );                
            }
            
            void onAppleAppLovinBannerDidClickAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidClickAd" );
            }
            
            void onAppleAppLovinBannerDidFailToDisplayAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinBannerDidExpandAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidExpandAd" );
            }
            
            void onAppleAppLovinBannerDidCollapseAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidCollapseAd" );
            }
            
            void onAppleAppLovinBannerDidPayRevenueForAd() override
            {
                this->call_cbs( "onAppleAppLovinBannerDidPayRevenueForAd" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_initBanner( const ConstString & _adUnitId, const ConstString & _placement, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinBannerProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinBannerProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initBanner( _adUnitId, _placement, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinInterstitialProvider
            : public PythonCallbackProvider<AppleAppLovinInterstitailProviderInterface>
        {
        public:
            PythonAppleAppLovinInterstitialProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleAppLovinInterstitailProviderInterface>( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinInterstitialDidLoadAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidLoadAd" );
            }
            
            void onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinInterstitialDidDisplayAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidDisplayAd" );
            }
            
            void onAppleAppLovinInterstitialDidClickAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidClickAd" );
            }
            
            void onAppleAppLovinInterstitialDidHideAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidHideAd" );
            }
            
            void onAppleAppLovinInterstitialDidFailToDisplayAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinInterstitialDidPayRevenueForAd() override
            {
                this->call_cbs( "onAppleAppLovinInterstitialDidPayRevenueForAd" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_initInterstitial( const ConstString & _adUnitId, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinInterstitialProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinInterstitialProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initInterstitial( _adUnitId, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinRewardedProvider
            : public PythonCallbackProvider<AppleAppLovinRewardedProviderInterface>
        {
        public:
            PythonAppleAppLovinRewardedProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleAppLovinRewardedProviderInterface>( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinRewardedDidLoadAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidLoadAd"];
            }
            
            void onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinRewardedDidDisplayAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidDisplayAd" );
            }
            
            void onAppleAppLovinRewardedDidClickAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidClickAd" );
            }
            
            void onAppleAppLovinRewardedDidHideAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidHideAd" );
            }
            
            void onAppleAppLovinRewardedDidFailToDisplayAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinRewardedDidStartRewardedVideoForAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidStartRewardedVideoForAd" );
            }
            
            void onAppleAppLovinRewardedDidCompleteRewardedVideoForAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidCompleteRewardedVideoForAd" );
            }
            
            void onAppleAppLovinRewardedDidRewardUserForAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidRewardUserForAd" );
            }
            
            void onAppleAppLovinRewardedDidPayRevenueForAd() override
            {
                this->call_cbs( "onAppleAppLovinRewardedDidPayRevenueForAd" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_initRewarded( const ConstString & _adUnitId, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinRewardedProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinRewardedProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initRewarded( _adUnitId, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canYouShowInterstitial( const ConstString & _adUnit, const ConstString & _placement )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canYouShowInterstitial( _adUnit, _placement );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_showInterstitial( const ConstString & _adUnit, const ConstString & _placement )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->showInterstitial( _adUnit, _placement );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canOfferRewarded( const ConstString & _adUnit, const ConstString & _placement )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canOfferRewarded( _adUnit, _placement );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canYouShowRewarded( const ConstString & _adUnit, const ConstString & _placement )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canYouShowRewarded( _adUnit, _placement );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_showRewarded( const ConstString & _adUnit, const ConstString & _placement )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->showRewarded( _adUnit, _placement );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_showBanner( const ConstString & _adUnit )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->showBanner( _adUnit );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_hideBanner( const ConstString & _adUnit )
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->hideBanner( _adUnit );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinConsentFlowProvider
            : public PythonCallbackProvider<AppleAppLovinConsentFlowProviderInterface>
        {
        public:
            PythonAppleAppLovinConsentFlowProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleAppLovinConsentFlowProviderInterface>( _cbs, _args )
            {
            }
            
            ~PythonAppleAppLovinConsentFlowProvider() override
            {
            }

        protected:
            void onAppleAppLovinConsentFlowShowSuccessful() override
            {
                this->call_cbs( "onAppleAppLovinConsentFlowShowSuccessful" );
            }
            
            void onAppleAppLovinConsentFlowShowFailed() override
            {
                this->call_cbs( "onAppleAppLovinConsentFlowShowFailed" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_loadAndShowCMPFlow( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinConsentFlowProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinConsentFlowProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            APPLE_APPLOVIN_SERVICE()
                ->loadAndShowCMPFlow( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_isConsentFlowUserGeographyGDPR()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->isConsentFlowUserGeographyGDPR();
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_showMediationDebugger()
        {
            APPLE_APPLOVIN_SERVICE()
                ->showMediationDebugger();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinScriptEmbedding::AppleAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinScriptEmbedding::~AppleAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleAppLovin", true );
        
        pybind::def_function_args( _kernel, "appleAppLovinInitBanner", &Detail::s_appleAppLovin_initBanner );
        pybind::def_function_args( _kernel, "appleAppLovinInitInterstitial", &Detail::s_appleAppLovin_initInterstitial );
        pybind::def_function_args( _kernel, "appleAppLovinInitRewarded", &Detail::s_appleAppLovin_initRewarded );
        pybind::def_function( _kernel, "appleAppLovinShowBanner", &Detail::s_appleAppLovin_showBanner );
        pybind::def_function( _kernel, "appleAppLovinHideBanner", &Detail::s_appleAppLovin_hideBanner );
        pybind::def_function( _kernel, "appleAppLovinCanYouShowInterstitial", &Detail::s_appleAppLovin_canYouShowInterstitial );
        pybind::def_function( _kernel, "appleAppLovinShowInterstitial", &Detail::s_appleAppLovin_showInterstitial );
        pybind::def_function( _kernel, "appleAppLovinCanOfferRewarded", &Detail::s_appleAppLovin_canOfferRewarded );
        pybind::def_function( _kernel, "appleAppLovinCanYouShowRewarded", &Detail::s_appleAppLovin_canYouShowRewarded );
        pybind::def_function( _kernel, "appleAppLovinShowRewarded", &Detail::s_appleAppLovin_showRewarded );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::s_appleAppLovin_loadAndShowCMPFlow );
        pybind::def_function( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", &Detail::s_appleAppLovin_isConsentFlowUserGeographyGDPR );
        pybind::def_function( _kernel, "appleAppLovinShowMediationDebugger", &Detail::s_appleAppLovin_showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinInitBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinInitInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinInitRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinHideBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

