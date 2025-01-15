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
        class PythonAppleAppLovinProvider
            : public AppleAppLovinProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }
            
            ~PythonAppleAppLovinProvider()
            {
            }

        protected:
            void onAppleAppLovinBannerRevenuePaid( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinBannerRevenuePaid", _params );
            }
            
            void onAppleAppLovinInterstitialRevenuePaid( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinInterstitialRevenuePaid" );
            }
            
            void onAppleAppLovinRewardedUserRewarded( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinRewardedUserRewarded", _params );
            }
            
            void onAppleAppLovinRewardedRevenuePaid( const Params & _params ) override
            {
                this->call_method( "onAppleAppLovinRewardedRevenuePaid", _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleAppLovinProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            APPLE_APPLOVIN_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * appleAppLovin_getBannerViewport( pybind::kernel_interface * _kernel, const ConstString & _adUnitId )
        {
            Viewport viewport;
            bool result = APPLE_APPLOVIN_SERVICE()
                ->getBannerViewport( &viewport );
            
            if( result == false )
            {
                return _kernel->ret_none();
            }
            
            PyObject * py_viewport = pybind::ptr( _kernel, viewport );
            
            return py_viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinInterstitialProvider
            : public AppleAppLovinInterstitialProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinInterstitialProvider( const pybind::object & _cb, const pybind::args & _args )
                : PythonCallbackProvider( _cb, _args )
            {
            }
            
            ~PythonAppleAppLovinInterstitialProvider() override
            {
            }

        protected:
            void onAppleAppLovinInterstitialShowCompleted( bool _successful, const Params & _params ) override
            {
                this->call_cb( _successful, _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_showInterstitial( const ConstString & _placement, const pybind::object & _cb, const pybind::args & _args )
        {
            AppleAppLovinInterstitialProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinInterstitialProvider>( MENGINE_DOCUMENT_PYBIND, _cb, _args );
            
            APPLE_APPLOVIN_SERVICE()
                ->showInterstitial( _placement, provider );
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleAppLovinRewardedProvider
            : public AppleAppLovinRewardedProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleAppLovinRewardedProvider( const pybind::object & _cb, const pybind::args & _args )
                : PythonCallbackProvider( _cb, _args )
            {
            }
            
            ~PythonAppleAppLovinRewardedProvider() override
            {
            }

        protected:
            void onAppleAppLovinRewardedShowCompleted( bool _successful, const Params & _params ) override
            {
                this->call_cb( _successful, _params );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_showRewarded( const ConstString & _placement, const pybind::object & _cb, const pybind::args & _args )
        {
            AppleAppLovinRewardedProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleAppLovinRewardedProvider>( MENGINE_DOCUMENT_PYBIND, _cb, _args );
            
            APPLE_APPLOVIN_SERVICE()
                ->showRewarded( _placement, provider );
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
        static void appleAppLovin_loadAndShowCMPFlow( const pybind::dict & _cbs, const pybind::args & _args )
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
        
        pybind::def_function_args( _kernel, "appleAppLovinSetProvider", &Detail::appleAppLovin_setProvider );
        
        pybind::def_functor( _kernel, "appleAppLovinShowBanner", service, &AppleAppLovinServiceInterface::showBanner );
        pybind::def_functor( _kernel, "appleAppLovinHideBanner", service, &AppleAppLovinServiceInterface::hideBanner );
        pybind::def_function_kernel( _kernel, "appleAppLovinGetBannerViewport", &Detail::appleAppLovin_getBannerViewport );
        
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowInterstitial", service,  &AppleAppLovinServiceInterface::canYouShowInterstitial );
        pybind::def_function_args( _kernel, "appleAppLovinShowInterstitial", &Detail::appleAppLovin_showInterstitial );
        
        pybind::def_functor( _kernel, "appleAppLovinCanOfferRewarded", service, &AppleAppLovinServiceInterface::canOfferRewarded );
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowRewarded", service, &AppleAppLovinServiceInterface::canYouShowRewarded );
        pybind::def_function_args( _kernel, "appleAppLovinShowRewarded", &Detail::appleAppLovin_showRewarded );
        
        pybind::def_functor( _kernel, "appleAppLovinHasSupportedCMP", service, &AppleAppLovinServiceInterface::hasSupportedCMP );
        pybind::def_functor( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", service, &AppleAppLovinServiceInterface::isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::appleAppLovin_loadAndShowCMPFlow );
        pybind::def_functor( _kernel, "appleAppLovinShowMediationDebugger", service, &AppleAppLovinServiceInterface::showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinSetProvider", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinHideBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinGetBannerViewport", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

