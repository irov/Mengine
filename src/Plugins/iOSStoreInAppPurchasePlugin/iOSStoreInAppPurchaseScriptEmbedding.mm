#include "iOSStoreInAppPurchaseScriptEmbedding.h"

#include "iOSStoreInAppPurchaseInterface.h"
#import "iOSStoreInAppPurchasePlugin.h"

#include "iOSStoreInAppPurchasePaymentTransaction.h"
#include "iOSStoreInAppPurchaseProduct.h"
#include "iOSStoreInAppPurchaseProductsRequest.h"

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
        class PythoniOSStoreInAppPurchasePaymentTransactionProvider
            : public iOSStoreInAppPurchasePaymentTransactionProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythoniOSStoreInAppPurchasePaymentTransactionProvider( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onPaymentQueueUpdatedTransactionPurchasing( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionPurchasing", _transaction );
            }

            void onPaymentQueueUpdatedTransactionPurchased( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionPurchased", _transaction );
            }

            void onPaymentQueueUpdatedTransactionFailed( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionFailed", _transaction );
            }

            void onPaymentQueueUpdatedTransactionRestored( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionRestored", _transaction );
            }

            void onPaymentQueueUpdatedTransactionDeferred( const iOSStoreInAppPurchasePaymentTransactionInterfacePtr & _transaction ) override
            {
                this->call_method( "onPaymentQueueUpdatedTransactionDeferred", _transaction );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreInAppPurchase_setPaymentTransactionProvider(const pybind::dict & _cbs, const pybind::args & _args )
        {
            iOSStoreInAppPurchasePaymentTransactionProviderInterfacePtr provider = Helper::makeFactorableUnique<PythoniOSStoreInAppPurchasePaymentTransactionProvider>( MENGINE_DOCUMENT_PYTHON, _cbs, _args );

            [[iOSStoreInAppPurchasePlugin sharedInstance] setPaymentTransactionProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreInAppPurchase_removePaymentTransactionProvider()
        {
            [[iOSStoreInAppPurchasePlugin sharedInstance] setPaymentTransactionProvider:nullptr];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreInAppPurchase_canMakePayments()
        {
            return [[iOSStoreInAppPurchasePlugin sharedInstance] canMakePayments];
        }
        //////////////////////////////////////////////////////////////////////////
        class PythoniOSStoreInAppPurchaseProductsResponse
            : public iOSStoreInAppPurchaseProductsResponseInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythoniOSStoreInAppPurchaseProductsResponse( const pybind::dict & _cbs, const pybind::args & _args )
                : PythonCallbackProvider( _cbs, _args )
            {
            }

        protected:
            void onProductResponse( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request, const VectoriOSStoreInAppPurchaseProducts & _products ) override
            {
                this->call_method( "onProductResponse", _request, _products );
            }

            void onProductFinish( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_method( "onProductFinish", _request );
            }

            void onProductFail( const iOSStoreInAppPurchaseProductsRequestInterfacePtr & _request ) override
            {
                this->call_method( "onProductFail", _request );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static iOSStoreInAppPurchaseProductsRequestInterfacePtr iOSStoreInAppPurchase_requestProducts( NSSet * _consumableIdentifiers, NSSet * _nonconsumableIdentifiers, const pybind::dict & _cbs, const pybind::args & _args )
        {
            iOSStoreInAppPurchaseProductsResponseInterfacePtr response = Helper::makeFactorableUnique<PythoniOSStoreInAppPurchaseProductsResponse>( MENGINE_DOCUMENT_PYTHON, _cbs, _args );

            iOSStoreInAppPurchaseProductsRequestInterfacePtr request = [[iOSStoreInAppPurchasePlugin sharedInstance] requestProducts:_consumableIdentifiers
                                                                                                               nonconsumableIdentifiers:_nonconsumableIdentifiers
                                                                                                                                   cb:response];

            return request;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreInAppPurchase_isOwnedProduct( NSString * _productIdentifier )
        {
            return [[iOSStoreInAppPurchasePlugin sharedInstance] isOwnedProduct:_productIdentifier];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreInAppPurchase_purchaseProduct( const iOSStoreInAppPurchaseProductInterfacePtr & _product )
        {
            bool successful = [[iOSStoreInAppPurchasePlugin sharedInstance] purchaseProduct:_product];

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreInAppPurchase_restoreCompletedTransactions()
        {
            [[iOSStoreInAppPurchasePlugin sharedInstance] restoreCompletedTransactions];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseScriptEmbedding::iOSStoreInAppPurchaseScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseScriptEmbedding::~iOSStoreInAppPurchaseScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSStoreInAppPurchaseScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseSetPaymentTransactionProvider", &Detail::iOSStoreInAppPurchase_setPaymentTransactionProvider );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRemovePaymentTransactionProvider", &Detail::iOSStoreInAppPurchase_removePaymentTransactionProvider );

        pybind::def_function( _kernel, "appleStoreInAppPurchaseCanMakePayments", &Detail::iOSStoreInAppPurchase_canMakePayments );
        pybind::def_function_args( _kernel, "appleStoreInAppPurchaseRequestProducts", &Detail::iOSStoreInAppPurchase_requestProducts );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseIsOwnedProduct", &Detail::iOSStoreInAppPurchase_isOwnedProduct );
        pybind::def_function( _kernel, "appleStoreInAppPurchasePurchaseProduct", &Detail::iOSStoreInAppPurchase_purchaseProduct );
        pybind::def_function( _kernel, "appleStoreInAppPurchaseRestoreCompletedTransactions", &Detail::iOSStoreInAppPurchase_restoreCompletedTransactions );

        pybind::interface_<iOSStoreInAppPurchasePaymentTransactionInterface, pybind::bases<Factorable>>( _kernel, "iOSStoreInAppPurchasePaymentTransactionInterface", true )
            .def( "getProductIdentifier", &iOSStoreInAppPurchasePaymentTransactionInterface::getProductIdentifier )
            .def( "finish", &iOSStoreInAppPurchasePaymentTransactionInterface::finish )
            ;

        pybind::interface_<iOSStoreInAppPurchaseProductInterface, pybind::bases<Factorable>>( _kernel, "iOSStoreInAppPurchaseProductInterface", true )
            .def( "getProductIdentifier", &iOSStoreInAppPurchaseProductInterface::getProductIdentifier )
            .def( "getProductTitle", &iOSStoreInAppPurchaseProductInterface::getProductTitle )
            .def( "getProductDescription", &iOSStoreInAppPurchaseProductInterface::getProductDescription )
            .def( "getProductCurrencyCode", &iOSStoreInAppPurchaseProductInterface::getProductCurrencyCode )
            .def( "getProductPriceFormatted", &iOSStoreInAppPurchaseProductInterface::getProductPriceFormatted )
            .def( "getProductPrice", &iOSStoreInAppPurchaseProductInterface::getProductPrice )
            ;

        pybind::interface_<iOSStoreInAppPurchaseProductsRequestInterface, pybind::bases<Factorable>>( _kernel, "iOSStoreInAppPurchaseProductsRequestInterface", true )
            .def( "cancel", &iOSStoreInAppPurchaseProductsRequestInterface::cancel )
            ;

        pybind::interface_<iOSStoreInAppPurchasePaymentTransaction, pybind::bases<iOSStoreInAppPurchasePaymentTransactionInterface>>( _kernel, "iOSStoreInAppPurchasePaymentTransaction", false )
            ;

        pybind::interface_<iOSStoreInAppPurchaseProduct, pybind::bases<iOSStoreInAppPurchaseProductInterface>>( _kernel,
            "iOSStoreInAppPurchaseProduct", false )
            ;

        pybind::interface_<iOSStoreInAppPurchaseProductsRequest, pybind::bases<iOSStoreInAppPurchaseProductsRequestInterface>>( _kernel, "iOSStoreInAppPurchaseProductsRequest", false )
            ;

        Helper::registerScriptWrapping<iOSStoreInAppPurchasePaymentTransaction>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<iOSStoreInAppPurchaseProduct>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<iOSStoreInAppPurchaseProductsRequest>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        pybind::registration_stl_vector_type_cast<VectoriOSStoreInAppPurchaseProducts>( _kernel );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchaseScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<VectoriOSStoreInAppPurchaseProducts>( _kernel );

        _kernel->remove_scope<iOSStoreInAppPurchasePaymentTransactionInterface>();
        _kernel->remove_scope<iOSStoreInAppPurchaseProductInterface>();
        _kernel->remove_scope<iOSStoreInAppPurchaseProductsRequestInterface>();

        Helper::unregisterScriptWrapping<iOSStoreInAppPurchasePaymentTransaction>();
        Helper::unregisterScriptWrapping<iOSStoreInAppPurchaseProduct>();
        Helper::unregisterScriptWrapping<iOSStoreInAppPurchaseProductsRequest>();
    }
    //////////////////////////////////////////////////////////////////////////
}
