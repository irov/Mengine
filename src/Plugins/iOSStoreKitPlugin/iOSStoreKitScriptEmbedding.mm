#include "iOSStoreKitScriptEmbedding.h"

#include "iOSStoreKitInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Plugins/iOSNativePythonPlugin/iOSNativePythonTypeCast.h"

#import "Environment/Python/ApplePythonProvider.h"

#import "iOSStoreKitPlugin.h"

@interface PythoniOSStoreKitProvider : ApplePythonProvider<iOSStoreKitProviderInterface>
@end

@implementation PythoniOSStoreKitProvider

- (void)onStoreKitProductsResponse:(NSArray<NSDictionary *> *)products {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitProductsResponse"];

    if (py_cb.is_callable() == false) {
        return;
    }

    NSArray * value = products;

    py_cb.call_args( value, self.m_args );
}

- (void)onStoreKitProductsFailed {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitProductsFailed"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onStoreKitTransaction:(NSDictionary *)transaction {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitTransaction"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( transaction, self.m_args );
}

- (void)onStoreKitTransactionFinished:(NSString *)productIdentifier transactionIdentifier:(NSString *)transactionIdentifier {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitTransactionFinished"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( productIdentifier, transactionIdentifier, self.m_args );
}

- (void)onStoreKitRestoreResult:(NSArray<NSDictionary *> *)transactions successful:(BOOL)successful {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitRestoreResult"];

    if (py_cb.is_callable() == false) {
        return;
    }

    NSArray * value = transactions;
    bool result = successful == YES;

    py_cb.call_args( value, result, self.m_args );
}

- (void)onStoreKitPurchaseCancelled:(NSString *)productIdentifier {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitPurchaseCancelled"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( productIdentifier, self.m_args );
}

- (void)onStoreKitPurchasePending:(NSString *)productIdentifier {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitPurchasePending"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( productIdentifier, self.m_args );
}

- (void)onStoreKitPurchaseFailed:(NSString *)productIdentifier {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitPurchaseFailed"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( productIdentifier, self.m_args );
}

- (void)onStoreKitEntitlementChanged:(NSString *)productIdentifier owned:(BOOL)owned {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitEntitlementChanged"];

    if (py_cb.is_callable() == false) {
        return;
    }

    bool value = owned == YES;

    py_cb.call_args( productIdentifier, value, self.m_args );
}

- (void)onStoreKitSubscriptionStatus:(NSString *)productIdentifier status:(NSDictionary *)status {
    pybind::object py_cb = [self getMethod:@"oniOSStoreKitSubscriptionStatus"];

    if (py_cb.is_callable() == false) {
        return;
    }

    py_cb.call_args( productIdentifier, status, self.m_args );
}

@end

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSStoreKitProviderInterface> provider = [[PythoniOSStoreKitProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSStoreKitPlugin sharedInstance] setProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_removeProvider()
        {
            [[iOSStoreKitPlugin sharedInstance] setProvider:nil];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreKit_canMakePayments()
        {
            if( [[iOSStoreKitPlugin sharedInstance] canMakePayments] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_requestProducts( NSArray * _consumableIdentifiers, NSArray * _nonConsumableIdentifiers )
        {
            [[iOSStoreKitPlugin sharedInstance] requestProducts:_consumableIdentifiers nonConsumableIdentifiers:_nonConsumableIdentifiers];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreKit_purchase( NSString * _productIdentifier, NSString * _accountToken )
        {
            if( [[iOSStoreKitPlugin sharedInstance] purchaseProduct:_productIdentifier accountToken:_accountToken] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_completeTransactionDelivery( NSString * _transactionIdentifier, bool _successful )
        {
            [[iOSStoreKitPlugin sharedInstance] completeTransactionDelivery:_transactionIdentifier successful:(_successful == true ? YES : NO)];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_restorePurchases()
        {
            [[iOSStoreKitPlugin sharedInstance] restorePurchases];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreKit_isOwnedProduct( NSString * _productIdentifier )
        {
            if( [[iOSStoreKitPlugin sharedInstance] isOwnedProduct:_productIdentifier] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreKit_querySubscriptionStatus( NSString * _productIdentifier )
        {
            [[iOSStoreKitPlugin sharedInstance] querySubscriptionStatus:_productIdentifier];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSStoreKit_showManageSubscriptions()
        {
            if( [[iOSStoreKitPlugin sharedInstance] showManageSubscriptions] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreKitScriptEmbedding::iOSStoreKitScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreKitScriptEmbedding::~iOSStoreKitScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSStoreKitScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "iOSStoreKitSetProvider", &Detail::iOSStoreKit_setProvider );
        pybind::def_function( _kernel, "iOSStoreKitRemoveProvider", &Detail::iOSStoreKit_removeProvider );
        pybind::def_function( _kernel, "iOSStoreKitCanMakePayments", &Detail::iOSStoreKit_canMakePayments );
        pybind::def_function( _kernel, "iOSStoreKitRequestProducts", &Detail::iOSStoreKit_requestProducts );
        pybind::def_function( _kernel, "iOSStoreKitPurchase", &Detail::iOSStoreKit_purchase );
        pybind::def_function( _kernel, "iOSStoreKitCompleteTransactionDelivery", &Detail::iOSStoreKit_completeTransactionDelivery );
        pybind::def_function( _kernel, "iOSStoreKitRestorePurchases", &Detail::iOSStoreKit_restorePurchases );
        pybind::def_function( _kernel, "iOSStoreKitIsOwnedProduct", &Detail::iOSStoreKit_isOwnedProduct );
        pybind::def_function( _kernel, "iOSStoreKitQuerySubscriptionStatus", &Detail::iOSStoreKit_querySubscriptionStatus );
        pybind::def_function( _kernel, "iOSStoreKitShowManageSubscriptions", &Detail::iOSStoreKit_showManageSubscriptions );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSStoreKitScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSStoreKitSetProvider", nullptr );
        _kernel->remove_from_module( "iOSStoreKitRemoveProvider", nullptr );
        _kernel->remove_from_module( "iOSStoreKitCanMakePayments", nullptr );
        _kernel->remove_from_module( "iOSStoreKitRequestProducts", nullptr );
        _kernel->remove_from_module( "iOSStoreKitPurchase", nullptr );
        _kernel->remove_from_module( "iOSStoreKitCompleteTransactionDelivery", nullptr );
        _kernel->remove_from_module( "iOSStoreKitRestorePurchases", nullptr );
        _kernel->remove_from_module( "iOSStoreKitIsOwnedProduct", nullptr );
        _kernel->remove_from_module( "iOSStoreKitQuerySubscriptionStatus", nullptr );
        _kernel->remove_from_module( "iOSStoreKitShowManageSubscriptions", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
