#include "AppleStoreInAppPurchaseScriptEmbedding.h"

#include "AppleStoreInAppPurchaseInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "AppleStoreInAppPurchasePaymentTransaction.h"
#include "AppleStoreInAppPurchaseProduct.h"
#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

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
            : public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchasePaymentQueueProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onPaymentQueueShouldContinueTransaction( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueShouldContinueTransaction", _transaction );
            }
            
            void onPaymentQueueShouldShowPriceConsent() override
            {
                this->call_cbs( "onPaymentQueueShouldShowPriceConsent" );
            }
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
            : public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchasePaymentTransactionProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onPaymentQueueUpdatedTransactionPurchasing( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueUpdatedTransactionPurchasing", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionPurchased( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueUpdatedTransactionPurchased", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionFailed( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueUpdatedTransactionFailed", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionRestored( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueUpdatedTransactionRestored", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionDeferred( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_cbs( "onPaymentQueueUpdatedTransactionDeferred", _transaction );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreInAppPurchase_setPaymentTransactionProvider(const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchasePaymentTransactionProvider>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

            APPLE_STOREINAPPPURCHASE_SERVICE()
                ->setPaymentTransactionProvider( provider );
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreInAppPurchase_removePaymentTransactionProvider()
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
            : public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonAppleStoreInAppPurchaseProductsResponse( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }
            
        protected:
            void onProductResponse( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request, const VectorAppleStoreInAppPurchaseProducts & _products ) override
            {
                this->call_cbs( "onProductResponse", _request, _products );
            }
            
            void onProductFinish( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_cbs( "onProductFinish", _request );
            }
            
            void onProductFail( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_cbs( "onProductFail", _request );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static AppleStoreInAppPurchaseProductsRequestInterfacePtr AppleStoreInAppPurchase_requestProducts(const VectorConstString & _productIdentifiers, const pybind::dict & _cbs, const pybind::args & _args )
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
        AppleStoreInAppPurchaseServiceInterface * service = APPLE_STOREINAPPPURCHASE_SERVICE();

        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseSetPaymentTransactionProvider", &Detail::AppleStoreInAppPurchase_setPaymentTransactionProvider );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRemovePaymentTransactionProvider", &Detail::AppleStoreInAppPurchase_removePaymentTransactionProvider );
        
        pybind::def_functor( _kernel, "appleStoreInAppPurchaseCanMakePayments", service, &AppleStoreInAppPurchaseServiceInterface::canMakePayments );
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseRequestProducts", &Detail::AppleStoreInAppPurchase_requestProducts );
        pybind::def_functor( _kernel, "appleStoreInAppPurchasePurchaseProduct", service, &AppleStoreInAppPurchaseServiceInterface::purchaseProduct );
        pybind::def_functor( _kernel, "appleStoreInAppPurchaseRestoreCompletedTransactions", service, &AppleStoreInAppPurchaseServiceInterface::restoreCompletedTransactions );

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
  
        pybind::interface_<AppleStoreInAppPurchasePaymentTransaction, pybind::bases<AppleStoreInAppPurchasePaymentTransactionInterface>>( _kernel, "AppleStoreInAppPurchasePaymentTransaction", false )
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

