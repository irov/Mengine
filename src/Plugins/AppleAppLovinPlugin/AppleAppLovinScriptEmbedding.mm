#include "AppleAppLovinScriptEmbedding.h"

#include "AppleAppLovinInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

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
        static void s_appleAppLovin_initBanner( const ConstString & _bannerAdUnit )
        {
            APPLE_APPLOVIN_SERVICE()
                ->initBanner( _bannerAdUnit );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_initInterstitial( const ConstString & _interstitialAdUnit )
        {
            APPLE_APPLOVIN_SERVICE()
                ->initInterstitial( _interstitialAdUnit );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_initRewarded( const ConstString & _rewardedAdUnit )
        {
            APPLE_APPLOVIN_SERVICE()
                ->initRewarded( _rewardedAdUnit );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canYouShowInterstitial()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canYouShowInterstitial();
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_showInterstitial()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->showInterstitial();
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canOfferRewarded()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canOfferRewarded();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_canYouShowRewarded()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->canYouShowRewarded();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_appleAppLovin_showRewarded()
        {
            bool result = APPLE_APPLOVIN_SERVICE()
                ->showRewarded();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_showBanner()
        {
            APPLE_APPLOVIN_SERVICE()
                ->showBanner();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appleAppLovin_hideBanner()
        {
            APPLE_APPLOVIN_SERVICE()
                ->hideBanner();
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
        
        pybind::def_function( _kernel, "appleAppLovinInitBanner", &Detail::s_appleAppLovin_initBanner );
        pybind::def_function( _kernel, "appleAppLovinInitInterstitial", &Detail::s_appleAppLovin_initInterstitial );
        pybind::def_function( _kernel, "appleAppLovinInitRewarded", &Detail::s_appleAppLovin_initRewarded );
        pybind::def_function( _kernel, "appleAppLovinCanYouShowInterstitial", &Detail::s_appleAppLovin_canYouShowInterstitial );
        pybind::def_function( _kernel, "appleAppLovinShowInterstitial", &Detail::s_appleAppLovin_showInterstitial );
        pybind::def_function( _kernel, "appleAppLovinCanOfferRewarded", &Detail::s_appleAppLovin_canOfferRewarded );
        pybind::def_function( _kernel, "appleAppLovinCanYouShowRewarded", &Detail::s_appleAppLovin_canYouShowRewarded );
        pybind::def_function( _kernel, "appleAppLovinShowRewarded", &Detail::s_appleAppLovin_showRewarded );
        pybind::def_function( _kernel, "appleAppLovinShowBanner", &Detail::s_appleAppLovin_showBanner );
        pybind::def_function( _kernel, "appleAppLovinHideBanner", &Detail::s_appleAppLovin_hideBanner );
        pybind::def_function( _kernel, "appleAppLovinShowMediationDebugger", &Detail::s_appleAppLovin_showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinInitBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinInitInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinInitRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinHasLoadedinterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinHasLoadedrewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinHideBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

