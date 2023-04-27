#include "AppleMARSDKScriptEmbedding.h"

#include "AppleMARSDKInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleMARSDKProvider
            : public AppleMARSDKProviderInterface
        {
        public:
            PythonAppleMARSDKProvider( pybind::kernel_interface * _kernel, const pybind::dict & _cbs, const pybind::args & _args )
                : m_kernel( _kernel )
                , m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
        protected:
            void onPlatformInit( const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onPlatformInit"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( d, m_args );
            }
            
            void onRealName( const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onRealName"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( d, m_args );
            }
            
            void onEventWithCode( int32_t _code, const Char * _msg ) override
            {
                pybind::object cb = m_cbs["onEventWithCode"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _code, _msg, m_args );
            }
            
            void onEventCustom( const Char * _eventName, const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onEventCustom"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( _eventName, d, m_args );
            }

        protected:
            void onUserLogin( const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onUserLogin"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( d, m_args );
            }

            void onUserLogout( const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onUserLogout"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( d, m_args );
            }

            void onPayPaid( const MARSDKResultParams & _params ) override
            {
                pybind::object cb = m_cbs["onPayPaid"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                pybind::dict d = this->makePyParams_( _params );
                
                cb.call_args( d, m_args );
            }
            
        protected:
            void onPropComplete( const ConstString & _orderId ) override
            {
                pybind::object cb = m_cbs["onPropComplete"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _orderId, m_args );
            }
            
            void onPropError( const ConstString & _orderId ) override
            {
                pybind::object cb = m_cbs["onPropError"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _orderId, m_args );
            }
            
        protected:
            void onPurchasedNonConsumable( const VectorConstString & _purchased ) override
            {
                pybind::object cb = m_cbs["onPurchasedNonConsumable"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _purchased, m_args );
            }
            
        protected:
            void onAdRewardedDidFailed() override
            {
                pybind::object cb = m_cbs["onAdRewardedDidFailed"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidLoaded() override
            {
                pybind::object cb = m_cbs.get("onAdRewardedDidLoaded");
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidShow() override
            {
                pybind::object cb = m_cbs["onAdRewardedDidShow"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidClicked() override
            {
                pybind::object cb = m_cbs["onAdRewardedDidClicked"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidClosed() override
            {
                pybind::object cb = m_cbs["onAdRewardedDidClosed"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidSkipped() override
            {
                pybind::object cb = m_cbs["onAdRewardedDidSkipped"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( m_args );
            }
            
            void onAdRewardedDidFinished( const Char * _itemName, uint32_t _itemNum ) override
            {
                pybind::object cb = m_cbs["onAdRewardedDidFinished"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _itemName, _itemNum, m_args );
            }
            
        protected:
            pybind::dict makePyParams_( const MARSDKResultParams & _params ) const
            {
                pybind::dict d( m_kernel );
                
                for( auto && [key, value] : _params )
                {
                    d[key] = value;
                }
                
                return d;
            }
            
        protected:
            pybind::kernel_interface * m_kernel;
            pybind::dict m_cbs;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleMARSDKProvider, AppleMARSDKProviderInterface> PythonAppleMARSDKProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_setProvider( pybind::kernel_interface * _kernel, const pybind::dict & _cbs, const pybind::args & _args )
        {
            PythonAppleMARSDKProviderPtr provider = Helper::makeFactorableUnique<PythonAppleMARSDKProvider>( MENGINE_DOCUMENT_PYBIND, _kernel, _cbs, _args );

            APPLE_MARSDK_SERVICE()
                ->setProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_login()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->login();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_logout()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->logout();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleMARSDK_switchAccount()
        {
            bool result = APPLE_MARSDK_SERVICE()
                ->switchAccount();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_requestNonConsumablePurchased()
        {
            APPLE_MARSDK_SERVICE()
                ->requestNonConsumablePurchased();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_submitExtendedData( const Char * _data )
        {
            APPLE_MARSDK_SERVICE()
                ->submitExtendedData( _data );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_submitPaymentData( const Char * _data )
        {
            APPLE_MARSDK_SERVICE()
                ->submitPaymentData( _data );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_propComplete( const ConstString & _orderId )
        {
            APPLE_MARSDK_SERVICE()
                ->propComplete( _orderId );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleMARSDK_showRewardVideoAd( const ConstString & _itemName, uint32_t _itemNum )
        {
            APPLE_MARSDK_SERVICE()
                ->showRewardVideoAd( _itemName, _itemNum );
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
            ->setAvailablePlugin( "AppleMARSDK", true );

        pybind::def_function_kernel_args( _kernel, "appleMARSDKSetProvider", &Detail::s_AppleMARSDK_setProvider );
        pybind::def_function( _kernel, "appleMARSDKLogin", &Detail::s_AppleMARSDK_login );
        pybind::def_function( _kernel, "appleMARSDKLogout", &Detail::s_AppleMARSDK_logout );
        pybind::def_function( _kernel, "appleMARSDKSwitchAccount", &Detail::s_AppleMARSDK_switchAccount );
        pybind::def_function( _kernel, "appleMARSDKRequestNonConsumablePurchased", &Detail::s_AppleMARSDK_requestNonConsumablePurchased );        
        pybind::def_function( _kernel, "appleMARSDKSubmitExtendedData", &Detail::s_AppleMARSDK_submitExtendedData );
        pybind::def_function( _kernel, "appleMARSDKSubmitPaymentData", &Detail::s_AppleMARSDK_submitPaymentData );
        pybind::def_function( _kernel, "appleMARSDKPropComplete", &Detail::s_AppleMARSDK_propComplete );
        pybind::def_function( _kernel, "appleMARSDKShowRewardVideoAd", &Detail::s_AppleMARSDK_showRewardVideoAd );
        
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

