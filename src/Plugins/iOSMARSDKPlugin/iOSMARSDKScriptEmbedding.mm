#include "iOSMARSDKScriptEmbedding.h"

#include "iOSMARSDKInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#include "Kernel/ConstString.h"

#import "Environment/Python/ApplePythonProvider.h"

#import "iOSMARSDKPlugin.h"

@interface PythoniOSMARSDKProvider : ApplePythonProvider<iOSMARSDKProviderInterface>
@end

@implementation PythoniOSMARSDKProvider

- (void)onPlatformInit:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onPlatformInit"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onRealName:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onRealName"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onEventWithCode:(NSInteger)code msg:(NSString *)msg {
    pybind::object py_cb = [self getMethod:@"onEventWithCode"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( code, msg, self.m_args );
}

- (void)onEventCustom:(NSString *)eventName params:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onEventCustom"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( eventName, params, self.m_args );
}

- (void)onUserLogin:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onUserLogin"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onUserLogout:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onUserLogout"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onPayPaid:(NSDictionary *)params {
    pybind::object py_cb = [self getMethod:@"onPayPaid"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)onPropComplete:(NSString *)orderId {
    pybind::object py_cb = [self getMethod:@"onPropComplete"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( orderId, self.m_args );
}

- (void)onPropError:(NSString *)orderId {
    pybind::object py_cb = [self getMethod:@"onPropError"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( orderId, self.m_args );
}

- (void)onPurchasedNonConsumable:(NSArray<NSString *> *)purchased {
    pybind::object py_cb = [self getMethod:@"onPurchasedNonConsumable"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( purchased, self.m_args );
}

- (void)onAdRewardedDidFailed {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidFailed"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidLoaded {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidLoaded"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidShow {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidShow"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidShowFailed {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidShowFailed"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidClicked {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidClicked"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidClosed {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidClosed"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidSkipped {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidSkipped"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)onAdRewardedDidFinished:(NSString *)itemName itemNum:(NSUInteger)itemNum {
    pybind::object py_cb = [self getMethod:@"onAdRewardedDidFinished"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( itemName, itemNum, self.m_args );
}

@end

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_setProvider( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSMARSDKProviderInterface> provider = [[PythoniOSMARSDKProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSMARSDKPlugin sharedInstance] setProvider:provider];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSMARSDK_login()
        {
            return [[iOSMARSDKPlugin sharedInstance] login];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSMARSDK_logout()
        {
            return [[iOSMARSDKPlugin sharedInstance] logout];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSMARSDK_switchAccount()
        {
            return [[iOSMARSDKPlugin sharedInstance] switchAccount];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_requestNonConsumablePurchased()
        {
            [[iOSMARSDKPlugin sharedInstance] requestNonConsumablePurchased];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_submitExtendedData(const Char * _data )
        {
            NSString * data = _data != nullptr ? [NSString stringWithUTF8String:_data] : nil;

            if( data == nil )
            {
                data = @"";
            }

            [[iOSMARSDKPlugin sharedInstance] submitExtendedData:data];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_submitPaymentData(const Char * _data )
        {
            NSString * data = _data != nullptr ? [NSString stringWithUTF8String:_data] : nil;

            if( data == nil )
            {
                data = @"";
            }

            [[iOSMARSDKPlugin sharedInstance] submitPaymentData:data];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_propComplete(const ConstString & _orderId )
        {
            NSString * orderId = [NSString stringWithUTF8String:_orderId.c_str()];

            if( orderId == nil )
            {
                orderId = @"";
            }

            [[iOSMARSDKPlugin sharedInstance] propComplete:orderId];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSMARSDK_showRewardVideoAd(const ConstString & _itemName, uint32_t _itemNum )
        {
            NSString * itemName = [NSString stringWithUTF8String:_itemName.c_str()];

            if( itemName == nil )
            {
                itemName = @"";
            }

            [[iOSMARSDKPlugin sharedInstance] showRewardVideoAd:itemName itemNum:_itemNum];
        }
        //////////////////////////////////////////////////////////////////////////
        static int64_t iOSMARSDK_getInternetDate()
        {
            return [[iOSMARSDKPlugin sharedInstance] getInternetDate];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSMARSDKScriptEmbedding::iOSMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSMARSDKScriptEmbedding::~iOSMARSDKScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSMARSDKScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "iOSMARSDKSetProvider", &Detail::iOSMARSDK_setProvider );
        pybind::def_function( _kernel, "iOSMARSDKLogin", &Detail::iOSMARSDK_login );
        pybind::def_function( _kernel, "iOSMARSDKLogout", &Detail::iOSMARSDK_logout );
        pybind::def_function( _kernel, "iOSMARSDKSwitchAccount", &Detail::iOSMARSDK_switchAccount );
        pybind::def_function( _kernel, "iOSMARSDKRequestNonConsumablePurchased", &Detail::iOSMARSDK_requestNonConsumablePurchased );
        pybind::def_function( _kernel, "iOSMARSDKSubmitExtendedData", &Detail::iOSMARSDK_submitExtendedData );
        pybind::def_function( _kernel, "iOSMARSDKSubmitPaymentData", &Detail::iOSMARSDK_submitPaymentData );
        pybind::def_function( _kernel, "iOSMARSDKPropComplete", &Detail::iOSMARSDK_propComplete );
        pybind::def_function( _kernel, "iOSMARSDKShowRewardVideoAd", &Detail::iOSMARSDK_showRewardVideoAd );
        pybind::def_function( _kernel, "iOSMARSDKGetInternetDate", &Detail::iOSMARSDK_getInternetDate );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSMARSDKScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSMARSDKSetProvider", nullptr );
        _kernel->remove_from_module( "iOSMARSDKLogin", nullptr );
        _kernel->remove_from_module( "iOSMARSDKLogout", nullptr );
        _kernel->remove_from_module( "iOSMARSDKSwitchAccount", nullptr );
        _kernel->remove_from_module( "iOSMARSDKRequestNonConsumablePurchased", nullptr );
        _kernel->remove_from_module( "iOSMARSDKSubmitExtendedData", nullptr );
        _kernel->remove_from_module( "iOSMARSDKSubmitPaymentData", nullptr );
        _kernel->remove_from_module( "iOSMARSDKPropComplete", nullptr );
        _kernel->remove_from_module( "iOSMARSDKShowRewardVideoAd", nullptr );
        _kernel->remove_from_module( "iOSMARSDKGetInternetDate", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
