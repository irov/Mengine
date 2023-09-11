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
        class PythonAppleAppLovinBannerProvider
            : public AppleAppLovinBannerProviderInterface
            , public Factorable
        {
        public:
            PythonAppleAppLovinBannerProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleAppLovinBannerProvider() override
            {
            }

        protected:
            void onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidStartAdRequestForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidLoadAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidLoadAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidFailToLoadAdForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidClickAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidClickAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidFailToDisplayAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidFailToDisplayAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidExpandAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidExpandAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidCollapseAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidCollapseAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinBannerDidPayRevenueForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinBannerDidPayRevenueForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
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
            : public AppleAppLovinInterstitailProviderInterface
            , public Factorable
        {
        public:
            PythonAppleAppLovinInterstitialProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleAppLovinInterstitialProvider() override
            {
            }

        protected:
            void onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidStartAdRequestForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidLoadAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidLoadAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidFailToLoadAdForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidDisplayAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidDisplayAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidClickAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidClickAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidHideAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidHideAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidFailToDisplayAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidFailToDisplayAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinInterstitialDidPayRevenueForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinInterstitialDidPayRevenueForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
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
            : public AppleAppLovinRewardedProviderInterface
            , public Factorable
        {
        public:
            PythonAppleAppLovinRewardedProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleAppLovinRewardedProvider() override
            {
            }

        protected:
            void onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidStartAdRequestForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidLoadAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidLoadAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidFailToLoadAdForAdUnitIdentifier"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidDisplayAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidDisplayAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidClickAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidClickAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidHideAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidHideAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidFailToDisplayAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidFailToDisplayAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidStartRewardedVideoForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidStartRewardedVideoForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidCompleteRewardedVideoForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidCompleteRewardedVideoForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidRewardUserForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidRewardUserForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAppleAppLovinRewardedDidPayRevenueForAd() override {
                pybind::object cb = m_cbs["onAppleAppLovinRewardedDidPayRevenueForAd"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
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
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

