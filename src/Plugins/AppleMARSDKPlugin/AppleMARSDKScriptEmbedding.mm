#include "AppleMARSDKScriptEmbedding.h"

#include "AppleMARSDKInterface.h"

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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleMARSDKProvider
            : public PythonCallbackProvider<AppleMARSDKProviderInterface>
        {
            DECLARE_FACTORABLE( PythonAppleMARSDKProvider );
            
        public:
            PythonAppleMARSDKProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider<AppleMARSDKProviderInterface>( _cbs, _args )
            {
            }
            
        protected:
            void onPlatformInit( const MapParams & _params ) override
            {
                this->call_cbs( "onPlatformInit", _params );
            }
            
            void onRealName( const MapParams & _params ) override
            {
                this->call_cbs( "onRealName", _params );
            }
            
            void onEventWithCode( int32_t _code, const Char * _msg ) override
            {
                this->call_cbs( "onEventWithCode", _code, _msg );
            }
            
            void onEventCustom( const Char * _eventName, const MapParams & _params ) override
            {
                this->call_cbs( "onEventCustom", _eventName, _params );
            }

        protected:
            void onUserLogin( const MapParams & _params ) override
            {
                this->call_cbs( "onUserLogin", _params );
            }

            void onUserLogout( const MapParams & _params ) override
            {
                this->call_cbs( "onUserLogout", _params );
            }

            void onPayPaid( const MapParams & _params ) override
            {
                this->call_cbs( "onPayPaid", _params );
            }
            
        protected:
            void onPropComplete( const ConstString & _orderId ) override
            {
                this->call_cbs( "onPropComplete", _orderId );
            }
            
            void onPropError( const ConstString & _orderId ) override
            {
                this->call_cbs( "onPropError", _orderId );
            }
            
        protected:
            void onPurchasedNonConsumable( const VectorConstString & _purchased ) override
            {
                this->call_cbs( "onPurchasedNonConsumable", _purchased );
            }
            
        protected:
            void onAdRewardedDidFailed() override
            {
                this->call_cbs( "onAdRewardedDidFailed" );
            }
            
            void onAdRewardedDidLoaded() override
            {
                this->call_cbs( "onAdRewardedDidLoaded" );
            }
            
            void onAdRewardedDidShow() override
            {
                this->call_cbs( "onAdRewardedDidShow" );
            }
            
            void onAdRewardedDidShowFailed() override
            {
                this->call_cbs( "onAdRewardedDidShowFailed" );
            }
            
            void onAdRewardedDidClicked() override
            {
                this->call_cbs( "onAdRewardedDidClicked" );
            }
            
            void onAdRewardedDidClosed() override
            {
                this->call_cbs( "onAdRewardedDidClosed" );
            }
            
            void onAdRewardedDidSkipped() override
            {
                this->call_cbs( "onAdRewardedDidSkipped" );
            }
            
            void onAdRewardedDidFinished( const Char * _itemName, uint32_t _itemNum ) override
            {
                this->call_cbs( "onAdRewardedDidFinished", _itemName, _itemNum );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleMARSDKProvider, AppleMARSDKProviderInterface> PythonAppleMARSDKProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_setProvider(pybind::kernel_interface * _kernel, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleMARSDKProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleMARSDKProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_MARSDK_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleMARSDK_login()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->login();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleMARSDK_logout()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->logout();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleMARSDK_switchAccount()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->switchAccount();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_requestNonConsumablePurchased()
        {
            APPLE_MARSDK_SERVICE()
                ->requestNonConsumablePurchased();
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_submitExtendedData(const Char * _data )
        {
            APPLE_MARSDK_SERVICE()
                ->submitExtendedData( _data );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_submitPaymentData(const Char * _data )
        {
            APPLE_MARSDK_SERVICE()
                ->submitPaymentData( _data );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_propComplete(const ConstString & _orderId )
        {
            APPLE_MARSDK_SERVICE()
                ->propComplete( _orderId );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleMARSDK_showRewardVideoAd(const ConstString & _itemName, uint32_t _itemNum )
        {
            APPLE_MARSDK_SERVICE()
                ->showRewardVideoAd( _itemName, _itemNum );
        }
        //////////////////////////////////////////////////////////////////////////
        static int64_t AppleMARSDK_getInternetDate()
        {
            int64_t date = APPLE_MARSDK_SERVICE()
                ->getInternetDate();
            
            return date;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKScriptEmbedding::AppleMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKScriptEmbedding::~AppleMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleMARSDK"), true );

        pybind::def_function_kernel_args( _kernel, "appleMARSDKSetProvider", &Detail::AppleMARSDK_setProvider );
        pybind::def_function( _kernel, "appleMARSDKLogin", &Detail::AppleMARSDK_login );
        pybind::def_function( _kernel, "appleMARSDKLogout", &Detail::AppleMARSDK_logout );
        pybind::def_function( _kernel, "appleMARSDKSwitchAccount", &Detail::AppleMARSDK_switchAccount );
        pybind::def_function( _kernel, "appleMARSDKRequestNonConsumablePurchased", &Detail::AppleMARSDK_requestNonConsumablePurchased );
        pybind::def_function( _kernel, "appleMARSDKSubmitExtendedData", &Detail::AppleMARSDK_submitExtendedData );
        pybind::def_function( _kernel, "appleMARSDKSubmitPaymentData", &Detail::AppleMARSDK_submitPaymentData );
        pybind::def_function( _kernel, "appleMARSDKPropComplete", &Detail::AppleMARSDK_propComplete );
        pybind::def_function( _kernel, "appleMARSDKShowRewardVideoAd", &Detail::AppleMARSDK_showRewardVideoAd );
        pybind::def_function( _kernel, "appleMARSDKGetInternetDate", &Detail::AppleMARSDK_getInternetDate );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleMARSDKSetProvider", nullptr );
        _kernel->remove_from_module( "appleMARSDKLogin", nullptr );
        _kernel->remove_from_module( "appleMARSDKLogout", nullptr );
        _kernel->remove_from_module( "appleMARSDKSwitchAccount", nullptr );
        _kernel->remove_from_module( "appleMARSDKSubmitExtendedData", nullptr );
        _kernel->remove_from_module( "appleMARSDKSubmitPaymentData", nullptr );
        _kernel->remove_from_module( "appleMARSDKPropComplete", nullptr );
        _kernel->remove_from_module( "appleMARSDKShowRewardVideoAd", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

