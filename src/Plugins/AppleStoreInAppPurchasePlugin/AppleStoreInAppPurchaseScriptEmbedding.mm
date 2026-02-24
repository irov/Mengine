#include "AppleStoreInAppPurchaseScriptEmbedding.h"

#include "AppleStoreInAppPurchaseInterface.h"
#import "AppleStoreInAppPurchaseApplicationDelegate.h"

#include "AppleStoreInAppPurchasePaymentTransaction.h"
#include "AppleStoreInAppPurchaseProduct.h"
#include "AppleStoreInAppPurchaseProductsRequest.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocument.h"
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
        class PythonAppleStoreInAppPurchasePaymentTransactionProvider
            : public AppleStoreInAppPurchasePaymentTransactionProviderInterface
            , public PythonCallbackProvider
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
                this->call_method( "onPaymentQueueUpdatedTransactionPurchasing", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionPurchased( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionPurchased", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionFailed( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionFailed", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionRestored( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionRestored", _transaction );
            }
            
            void onPaymentQueueUpdatedTransactionDeferred( const AppleStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionDeferred", _transaction );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreInAppPurchase_setPaymentTransactionProvider(const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchasePaymentTransactionProviderInterfacePtr provider = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchasePaymentTransactionProvider>( MENGINE_DOCUMENT_PYTHON, _cbs, _args );

            [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] setPaymentTransactionProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreInAppPurchase_removePaymentTransactionProvider()
        {
            [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] setPaymentTransactionProvider:nullptr];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleStoreInAppPurchase_canMakePayments()
        {
            return [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] canMakePayments];
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleStoreInAppPurchaseProductsResponse
            : public AppleStoreInAppPurchaseProductsResponseInterface
            , public PythonCallbackProvider
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
                this->call_method( "onProductResponse", _request, _products );
            }
            
            void onProductFinish( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_method( "onProductFinish", _request );
            }
            
            void onProductFail( const AppleStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_method( "onProductFail", _request );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static AppleStoreInAppPurchaseProductsRequestInterfacePtr AppleStoreInAppPurchase_requestProducts( NSSet * _consumableIdentifiers, NSSet * _nonconsumableIdentifiers, const pybind::dict & _cbs, const pybind::args & _args )
        {
            AppleStoreInAppPurchaseProductsResponseInterfacePtr response = Helper::makeFactorableUnique<PythonAppleStoreInAppPurchaseProductsResponse>( MENGINE_DOCUMENT_PYTHON, _cbs, _args );
            
            AppleStoreInAppPurchaseProductsRequestInterfacePtr request = [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] requestProducts:_consumableIdentifiers
                                                                                                               nonconsumableIdentifiers:_nonconsumableIdentifiers
                                                                                                                                   cb:response];
            
            return request;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleStoreInAppPurchase_isOwnedProduct( NSString * _productIdentifier )
        {
            return [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] isOwnedProduct:_productIdentifier];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleStoreInAppPurchase_purchaseProduct( const AppleStoreInAppPurchaseProductInterfacePtr & _product )
        {
            bool successful = [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] purchaseProduct:_product];
            
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreInAppPurchase_restoreCompletedTransactions()
        {
            [[AppleStoreInAppPurchaseApplicationDelegate sharedInstance] restoreCompletedTransactions];
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
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseSetPaymentTransactionProvider", &Detail::AppleStoreInAppPurchase_setPaymentTransactionProvider );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRemovePaymentTransactionProvider", &Detail::AppleStoreInAppPurchase_removePaymentTransactionProvider );
        
        pybind::def_function( _kernel, "appleStoreInAppPurchaseCanMakePayments", &Detail::AppleStoreInAppPurchase_canMakePayments );
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseRequestProducts", &Detail::AppleStoreInAppPurchase_requestProducts );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseIsOwnedProduct", &Detail::AppleStoreInAppPurchase_isOwnedProduct );
        pybind::def_function( _kernel, "appleStoreInAppPurchasePurchaseProduct", &Detail::AppleStoreInAppPurchase_purchaseProduct );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRestoreCompletedTransactions", &Detail::AppleStoreInAppPurchase_restoreCompletedTransactions );

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

