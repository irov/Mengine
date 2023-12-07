#include "AppleStoreInAppPurchaseScriptEmbedding.h"

#include "AppleStoreInAppPurchaseInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "AppleStoreInAppPurchasePaymentTransaction.h"
#include "AppleStoreInAppPurchaseProduct.h"
#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleStoreInAppPurchasePaymentQueueProvider
            : public AppleStoreInAppPurchasePaymentQueueProviderInterface
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchasePaymentQueueProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleStoreInAppPurchasePaymentQueueProvider() override
            {
            }

        protected:
            void onPaymentQueueShouldContinueTransaction( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueShouldContinueTransaction"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }
            
            void onPaymentQueueShouldShowPriceConsent() override
            {
                pybind::object cb = m_cbs["onPaymentQueueShouldShowPriceConsent"];
                
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
        static void s_AppleStoreInAppPurchase_setPaymentQueueProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchasePaymentQueueProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchasePaymentQueueProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_STOREINAPPPURCHASE_SERVICE()
                ->setPaymentQueueProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleStoreInAppPurchasePaymentTransactionProvider
            : public AppleStoreInAppPurchasePaymentTransactionProviderInterface
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchasePaymentTransactionProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleStoreInAppPurchasePaymentTransactionProvider() override
            {                
            }

        protected:
            void onPaymentQueueUpdatedTransactionPurchasing( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueUpdatedTransactionPurchasing"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }
            
            void onPaymentQueueUpdatedTransactionPurchased( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueUpdatedTransactionPurchased"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }
            
            void onPaymentQueueUpdatedTransactionFailed( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueUpdatedTransactionFailed"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }
            
            void onPaymentQueueUpdatedTransactionRestored( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueUpdatedTransactionRestored"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }
            
            void onPaymentQueueUpdatedTransactionDeferred( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                pybind::object cb = m_cbs["onPaymentQueueUpdatedTransactionDeferred"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _transaction, m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleStoreInAppPurchase_setPaymentTransactionProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchasePaymentTransactionProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_STOREINAPPPURCHASE_SERVICE()
                ->setPaymentTransactionProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleStoreInAppPurchase_removePaymentTransactionProvider()
        {
            APPLE_STOREINAPPPURCHASE_SERVICE()
                ->setPaymentTransactionProvider( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleStoreInAppPurchase_canMakePayments()
        {
            bool result = APPLE_STOREINAPPPURCHASE_SERVICE()
                ->canMakePayments();
            
            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleStoreInAppPurchaseProductsResponse
            : public AppleStoreInAppPurchaseProductsResponseInterface
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchaseProductsResponse( const pybind::dict & _cbs, const pybind::args & _args )
                : m_cbs( _cbs )
                , m_args( _args )
            {
            }
            
            ~PythonAppleStoreInAppPurchaseProductsResponse() override
            {
            }
            
        protected:
            void onProductResponse( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request, const VectorAppleStoreInAppPurchaseProducts & _products ) override
            {
                pybind::object cb = m_cbs["onProductResponse"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _request, _products, m_args );
            }
            
            void onProductFinish( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                pybind::object cb = m_cbs["onProductFinish"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _request, m_args );
            }
            
            void onProductFail( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                pybind::object cb = m_cbs["onProductFail"];
                
                if( cb.is_none() == true )
                {
                    return;
                }
                
                cb.call_args( _request, m_args );
            }

        protected:
            pybind::dict m_cbs;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static AppleStoreInAppPurchaseProductsRequestInterfacePtr s_AppleStoreInAppPurchase_requestProducts( const VectorConstString & _productIdentifiers, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchaseProductsResponseInterfacePtr response = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchaseProductsResponse>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );
            
            AppleStoreInAppPurchaseProductsRequestInterfacePtr request = APPLE_STOREINAPPPURCHASE_SERVICE()
                ->requestProducts( _productIdentifiers, response );
            
            return request;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleStoreInAppPurchase_purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product )
        {
            bool successful = APPLE_STOREINAPPPURCHASE_SERVICE()
                ->purchaseProduct( _product );
            
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleStoreInAppPurchase_restoreCompletedTransactions()
        {
            APPLE_STOREINAPPPURCHASE_SERVICE()
                ->restoreCompletedTransactions();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseScriptEmbedding::AppleStoreInAppPurchaseScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseScriptEmbedding::~AppleStoreInAppPurchaseScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchaseScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleStoreInAppPurchase", true );

        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseSetPaymentTransactionProvider", &Detail::s_AppleStoreInAppPurchase_setPaymentTransactionProvider );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRemovePaymentTransactionProvider", &Detail::s_AppleStoreInAppPurchase_removePaymentTransactionProvider );
        
        pybind::def_function( _kernel, "appleStoreInAppPurchaseCanMakePayments", &Detail::s_AppleStoreInAppPurchase_canMakePayments );
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseRequestProducts", &Detail::s_AppleStoreInAppPurchase_requestProducts );
        pybind::def_function( _kernel, "appleStoreInAppPurchasePurchaseProduct", &Detail::s_AppleStoreInAppPurchase_purchaseProduct );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRestoreCompletedTransactions", &Detail::s_AppleStoreInAppPurchase_restoreCompletedTransactions );

        pybind::interface_<AppleStoreInAppPurchasePaymentTransactionInterface, pybind::bases<Factorable>>( _kernel, "AppleStoreInAppPurchasePaymentTransactionInterface", true )
            .def( "getProductIdentifier", &AppleStoreInAppPurchasePaymentTransactionInterface::getProductIdentifier )
            .def( "finish", &AppleStoreInAppPurchasePaymentTransactionInterface::finish )
            ;
        
        pybind::interface_<AppleStoreInAppPurchaseProductInterface, pybind::bases<Factorable>>( _kernel, "AppleStoreInAppPurchaseProductInterface", true )
            .def( "getProductIdentifier", &AppleStoreInAppPurchaseProductInterface::getProductIdentifier )
            .def( "getProductTitle", &AppleStoreInAppPurchaseProductInterface::getProductTitle )
            .def( "getProductDescription", &AppleStoreInAppPurchaseProductInterface::getProductDescription )
            .def( "getProductCurrencyCode", &AppleStoreInAppPurchaseProductInterface::getProductCurrencyCode )
            .def( "getProductPriceFormatted", &AppleStoreInAppPurchaseProductInterface::getProductPriceFormatted )
            .def( "getProductPrice", &AppleStoreInAppPurchaseProductInterface::getProductPrice )
            ;
        
        pybind::interface_<AppleStoreInAppPurchaseProductsRequestInterface, pybind::bases<Factorable>>( _kernel, "AppleStoreInAppPurchaseProductsRequestInterface", true )
            .def( "cancel", &AppleStoreInAppPurchaseProductsRequestInterface::cancel )
            ;
  
        pybind::interface_<AppleStoreInAppPurchasePaymentTransaction, pybind::bases<AppleStoreInAppPurchasePaymentTransactionInterface>>( _kernel,
            "AppleStoreInAppPurchasePaymentTransaction", false )
            ;
        
        pybind::interface_<AppleStoreInAppPurchaseProduct, pybind::bases<AppleStoreInAppPurchaseProductInterface>>( _kernel,
            "AppleStoreInAppPurchaseProduct", false )
            ;
        
        pybind::interface_<AppleStoreInAppPurchaseProductsRequest, pybind::bases<AppleStoreInAppPurchaseProductsRequestInterface>>( _kernel, "AppleStoreInAppPurchaseProductsRequest", false )
            ;
        
        Helper::registerScriptWrapping<AppleStoreInAppPurchasePaymentTransaction>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<AppleStoreInAppPurchaseProduct>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<AppleStoreInAppPurchaseProductsRequest>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        
        pybind::registration_stl_vector_type_cast<VectorAppleStoreInAppPurchaseProducts>( _kernel );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<VectorAppleStoreInAppPurchaseProducts>( _kernel );
        
        _kernel->remove_scope<AppleStoreInAppPurchasePaymentTransactionInterface>();
        _kernel->remove_scope<AppleStoreInAppPurchaseProductInterface>();
        _kernel->remove_scope<AppleStoreInAppPurchaseProductsRequestInterface>();
        
        Helper::unregisterScriptWrapping<AppleStoreInAppPurchasePaymentTransaction>();
        Helper::unregisterScriptWrapping<AppleStoreInAppPurchaseProduct>();
        Helper::unregisterScriptWrapping<AppleStoreInAppPurchaseProductsRequest>();
    }
    //////////////////////////////////////////////////////////////////////////
}

