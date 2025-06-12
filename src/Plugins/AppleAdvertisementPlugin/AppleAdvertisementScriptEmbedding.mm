#include "AppleAdvertisementScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#import "Environment/Python/ApplePythonProvider.h"


#include "AppleAdvertisementApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"


@interface PythonAppleAdvertisementBannerCallback : ApplePythonProvider<AppleAdvertisementCallbackInterface>
@end

@implementation PythonAppleAdvertisementBannerCallback

- (void)onAppleAdvertisementShowSuccess:(NSString *)placement {
    //Empty
}

- (void)onAppleAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    //Empty
}

- (void)onAppleAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };

    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementRevenuePaid"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    //Empty
}

@end

@interface PythonAppleAdvertisementInterstitialCallback : ApplePythonProvider<AppleAdvertisementCallbackInterface>
@end

@implementation PythonAppleAdvertisementInterstitialCallback

- (void)onAppleAdvertisementShowSuccess:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };
        
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementShowSuccess"];
        
    if( py_cb.is_callable() == false )
    {
        return;
    }
        
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    NSDictionary * params = @{
        @"placement": placement,
        @"errorCode": @(code)
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementShowFailed"];
        
    if( py_cb.is_callable() == false )
    {
        return;
    }
        
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementRevenuePaid"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    //Empty
}

@end

@interface PythonAppleAdvertisementRewardedCallback : ApplePythonProvider<AppleAdvertisementCallbackInterface>
@end

@implementation PythonAppleAdvertisementRewardedCallback

- (void)onAppleAdvertisementShowSuccess:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementShowSuccess"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    NSDictionary * params = @{
        @"placement": placement,
        @"errorCode": @(code)
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementShowFailed"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementRevenuePaid"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

- (void)onAppleAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    NSDictionary * params = @{
        @"placement": placement,
        @"label": label,
        @"amount": @(amount)
    };
    
    pybind::object py_cb = [self getMethod:@"onAppleAdvertisementUserRewarded"];
    
    if( py_cb.is_callable() == false )
    {
        return;
    }
    
    py_cb.call_args( params, self.m_args );
}

@end

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setBannerCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleAdvertisementCallbackInterface> callback = [[PythonAppleAdvertisementBannerCallback alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAdvertisementApplicationDelegate sharedInstance] setBannerCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setInterstitialCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleAdvertisementCallbackInterface> callback = [[PythonAppleAdvertisementInterstitialCallback alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAdvertisementApplicationDelegate sharedInstance] setInterstitialCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setRewardedCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleAdvertisementCallbackInterface> callback = [[PythonAppleAdvertisementRewardedCallback alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAdvertisementApplicationDelegate sharedInstance] setRewardedCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasBanner()
        {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] hasBanner] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showBanner()
        {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] showBanner] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hideBanner()
        {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] hideBanner] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * appleAdvertisement_getBannerWidth( pybind::kernel_interface * _kernel )
        {
            uint32_t width;
            uint32_t height;
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] getBannerWidth:&width height:&height] == NO) {
                return _kernel->ret_none();
            }
            
            PyObject * py_width = pybind::ptr( _kernel, width );
            
            return py_width;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * appleAdvertisement_getBannerHeight( pybind::kernel_interface * _kernel )
        {
            uint32_t width;
            uint32_t height;
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] getBannerWidth:&width height:&height] == NO) {
                return _kernel->ret_none();
            }
            
            PyObject * py_height = pybind::ptr( _kernel, height );
            
            return py_height;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasInterstitial() {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] hasInterstitial] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canYouShowInterstitial( NSString * _placement ) {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] canYouShowInterstitial:_placement] == NO) {
                return false;
            }
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showInterstitial( NSString * _placement ) {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] showInterstitial:_placement] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasRewarded() {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] hasRewarded] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canOfferRewarded( NSString * _placement ) {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] canOfferRewarded:_placement] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canYouShowRewarded( NSString * _placement ) {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] canYouShowRewarded:_placement] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showRewarded( NSString * _placement ) {
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] showRewarded:_placement] == NO) {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementScriptEmbedding::AppleAdvertisementScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementScriptEmbedding::~AppleAdvertisementScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "appleAdvertisementSetBannerCallback", &Detail::appleAdvertisement_setBannerCallback );
        pybind::def_function_args( _kernel, "appleAdvertisementSetInterstitialCallback", &Detail::appleAdvertisement_setInterstitialCallback );
        pybind::def_function_args( _kernel, "appleAdvertisementSetRewardedCallback", &Detail::appleAdvertisement_setRewardedCallback );
        
        pybind::def_function( _kernel, "appleAdvertisementHasBanner", &Detail::appleAdvertisement_hasBanner );
        pybind::def_function( _kernel, "appleAdvertisementShowBanner", &Detail::appleAdvertisement_showBanner );
        pybind::def_function( _kernel, "appleAdvertisementHideBanner", &Detail::appleAdvertisement_hideBanner );
        pybind::def_function_kernel( _kernel, "appleAdvertisementGetBannerWidth", &Detail::appleAdvertisement_getBannerWidth );
        pybind::def_function_kernel( _kernel, "appleAdvertisementGetBannerHeight", &Detail::appleAdvertisement_getBannerHeight );
        
        pybind::def_function( _kernel, "appleAdvertisementHasInterstitial", &Detail::appleAdvertisement_hasInterstitial );
        pybind::def_function( _kernel, "appleAdvertisementCanYouShowInterstitial", &Detail::appleAdvertisement_canYouShowInterstitial );
        pybind::def_function( _kernel, "appleAdvertisementShowInterstitial", &Detail::appleAdvertisement_showInterstitial );
        
        pybind::def_function( _kernel, "appleAdvertisementHasRewarded", &Detail::appleAdvertisement_hasRewarded );
        pybind::def_function( _kernel, "appleAdvertisementCanOfferRewarded", &Detail::appleAdvertisement_canOfferRewarded );
        pybind::def_function( _kernel, "appleAdvertisementCanYouShowRewarded", &Detail::appleAdvertisement_canYouShowRewarded );
        pybind::def_function( _kernel, "appleAdvertisementShowRewarded", &Detail::appleAdvertisement_showRewarded );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAdvertisementSetBannerCallback", nullptr );
        _kernel->remove_from_module( "appleAdvertisementSetInterstitialCallback", nullptr );
        _kernel->remove_from_module( "appleAdvertisementSetRewardedCallback", nullptr );
        _kernel->remove_from_module( "appleAdvertisementShowBanner", nullptr );
        _kernel->remove_from_module( "appleAdvertisementHideBanner", nullptr );
        _kernel->remove_from_module( "appleAdvertisementGetBannerWidth", nullptr );
        _kernel->remove_from_module( "appleAdvertisementGetBannerHeight", nullptr );
        _kernel->remove_from_module( "appleAdvertisementCanYouShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAdvertisementShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAdvertisementCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementShowRewarded", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

