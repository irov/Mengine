#include "AppleAppLovinScriptEmbedding.h"

#include "AppleAppLovinInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinBannerProvider
            : public AppleAppLovinBannerProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinBannerProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinBannerDidLoadAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidLoadAd" );
            }
            
            void onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinBannerDidDisplayAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidDisplayAd" );
            }
            
            void onAppleAppLovinBannerDidHideAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidHideAd" );
            }
            
            void onAppleAppLovinBannerDidClickAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidClickAd" );
            }
            
            void onAppleAppLovinBannerDidFailToDisplayAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinBannerDidExpandAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidExpandAd" );
            }
            
            void onAppleAppLovinBannerDidCollapseAd() override
            {
                this->call_method( "onAppleAppLovinBannerDidCollapseAd" );
            }
            
            void onAppleAppLovinBannerDidPayRevenueForAd(const Params & _params) override
            {
                this->call_method( "onAppleAppLovinBannerDidPayRevenueForAd", _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_initBanner(const ConstString & _adUnitId, const ConstString & _placement, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinBannerProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinBannerProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initBanner( _adUnitId, _placement, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * appleAppLovin_getBannerViewport(pybind::kernel_interface * _kernel, const ConstString & _adUnitId )
        {
            Viewport viewport;
            bool result = APPLE_APPLOVIN_SERVICE()
                ->getBannerViewport( _adUnitId, &viewport );
            
            if( result == false )
            {
                return _kernel->ret_none();
            }
            
            PyObject * py_viewport = pybind::ptr( _kernel, viewport );
            
            return py_viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinInterstitialProvider
            : public AppleAppLovinInterstitailProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinInterstitialProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinInterstitialDidLoadAd() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidLoadAd" );
            }
            
            void onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinInterstitialDidDisplayAd() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidDisplayAd" );
            }
            
            void onAppleAppLovinInterstitialDidClickAd() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidClickAd" );
            }
            
            void onAppleAppLovinInterstitialDidHideAd() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidHideAd" );
            }
            
            void onAppleAppLovinInterstitialDidFailToDisplayAd() override
            {
                this->call_method( "onAppleAppLovinInterstitialDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinInterstitialDidPayRevenueForAd( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinInterstitialDidPayRevenueForAd", _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_initInterstitial(const ConstString & _adUnitId, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinInterstitialProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinInterstitialProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initInterstitial( _adUnitId, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinRewardedProvider
            : public AppleAppLovinRewardedProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinRewardedProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinRewardedDidLoadAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidLoadAd" );
            }
            
            void onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier() override
            {
                this->call_method( "onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier" );
            }
            
            void onAppleAppLovinRewardedDidDisplayAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidDisplayAd" );
            }
            
            void onAppleAppLovinRewardedDidClickAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidClickAd" );
            }
            
            void onAppleAppLovinRewardedDidHideAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidHideAd" );
            }
            
            void onAppleAppLovinRewardedDidFailToDisplayAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidFailToDisplayAd" );
            }
            
            void onAppleAppLovinRewardedDidStartRewardedVideoForAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidStartRewardedVideoForAd" );
            }
            
            void onAppleAppLovinRewardedDidCompleteRewardedVideoForAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidCompleteRewardedVideoForAd" );
            }
            
            void onAppleAppLovinRewardedDidRewardUserForAd() override
            {
                this->call_method( "onAppleAppLovinRewardedDidRewardUserForAd" );
            }
            
            void onAppleAppLovinRewardedDidPayRevenueForAd( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinRewardedDidPayRevenueForAd", _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_initRewarded(const ConstString & _adUnitId, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinRewardedProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinRewardedProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            bool result = APPLE_APPLOVIN_SERVICE()
                ->initRewarded( _adUnitId, provider );
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinConsentFlowProvider
            : public AppleAppLovinConsentFlowProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinConsentFlowProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }
            
            ~PythonAppleAppLovinConsentFlowProvider() override
            {
            }

        protected:
            void onAppleAppLovinConsentFlowShowSuccessful() override
            {
                this->call_method( "onAppleAppLovinConsentFlowShowSuccessful" );
            }
            
            void onAppleAppLovinConsentFlowShowFailed() override
            {
                this->call_method( "onAppleAppLovinConsentFlowShowFailed" );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_loadAndShowCMPFlow(const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinConsentFlowProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinConsentFlowProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            APPLE_APPLOVIN_SERVICE()
                ->loadAndShowCMPFlow( provider );
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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleAppLovin"), true );
        
        AppleAppLovinServiceInterface * service = APPLE_APPLOVIN_SERVICE();
        
        pybind::def_function_args( _kernel, "appleAppLovinInitBanner", &Detail::appleAppLovin_initBanner );
        pybind::def_functor( _kernel, "appleAppLovinShowBanner", service, &AppleAppLovinServiceInterface::showBanner );
        pybind::def_functor( _kernel, "appleAppLovinHideBanner", service, &AppleAppLovinServiceInterface::hideBanner );
        pybind::def_function_kernel( _kernel, "appleAppLovinGetBannerViewport", &Detail::appleAppLovin_getBannerViewport );
        
        pybind::def_function_args( _kernel, "appleAppLovinInitInterstitial", &Detail::appleAppLovin_initInterstitial );
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowInterstitial", service,  &AppleAppLovinServiceInterface::canYouShowInterstitial );
        pybind::def_functor( _kernel, "appleAppLovinShowInterstitial", service, &AppleAppLovinServiceInterface::showInterstitial );
        
        pybind::def_function_args( _kernel, "appleAppLovinInitRewarded", &Detail::appleAppLovin_initRewarded );
        pybind::def_functor( _kernel, "appleAppLovinCanOfferRewarded", service, &AppleAppLovinServiceInterface::canOfferRewarded );
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowRewarded", service, &AppleAppLovinServiceInterface::canYouShowRewarded );
        pybind::def_functor( _kernel, "appleAppLovinShowRewarded", service, &AppleAppLovinServiceInterface::showRewarded );
        
        pybind::def_functor( _kernel, "appleAppLovinHasSupportedCMP", service, &AppleAppLovinServiceInterface::hasSupportedCMP );
        pybind::def_functor( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", service, &AppleAppLovinServiceInterface::isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::appleAppLovin_loadAndShowCMPFlow );
        pybind::def_functor( _kernel, "appleAppLovinShowMediationDebugger", service, &AppleAppLovinServiceInterface::showMediationDebugger );

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
        _kernel->remove_from_module( "appleAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

