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


@interface PythonAppleAdvertisementCallback : ApplePythonProvider<AppleAdvertisementCallbackInterface>
@end

@implementation PythonAppleAdvertisementCallback

- (void)onAppleAdvertisementShowSuccess:(iOSAdFormat *)format withPlacement:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };
    
    if (format == iOSAdFormat.ADFORMAT_INTERSTITIAL) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinInterstitialShowSuccess"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinRewardedShowSuccess"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    }
}

- (void)onAppleAdvertisementShowFailed:(iOSAdFormat *)format withPlacement:(NSString *)placement withError:(NSInteger)code {
    NSDictionary * params = @{
        @"placement": placement,
        @"errorCode": @(code)
    };
    
    if (format == iOSAdFormat.ADFORMAT_INTERSTITIAL) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinInterstitialShowFailed"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinRewardedShowFailed"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    }
}

- (void)onAppleAdvertisementRevenuePaid:(iOSAdFormat *)format withPlacement:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };
    
    if (format == iOSAdFormat.ADFORMAT_BANNER) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinBannerRevenuePaid"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_INTERSTITIAL) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinInterstitialRevenuePaid"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinRewardedRevenuePaid"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    }
}

- (void)onAppleAdvertisementUserRewarded:(iOSAdFormat *)format withPlacement:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    NSDictionary * params = @{
        @"placement": placement,
        @"label": label,
        @"amount": @(amount)
    };
    
    if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = [self getMethod:@"onAppleAppLovinRewardedUserRewarded"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    }
}

@end

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setAdvertisementCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            PythonAppleAdvertisementCallback * response = [[PythonAppleAdvertisementCallback alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAdvertisementApplicationDelegate sharedInstance] setAdvertisementCallback:response];
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleAdvertisement"), true );
        
        pybind::def_function_args( _kernel, "appleAdvertisementSetAdvertisementCallback", &Detail::appleAdvertisement_setAdvertisementCallback );
        
        pybind::def_function( _kernel, "appleAdvertisementShowBanner", &Detail::appleAdvertisement_showBanner );
        pybind::def_function( _kernel, "appleAdvertisementHideBanner", &Detail::appleAdvertisement_hideBanner );
        pybind::def_function_kernel( _kernel, "appleAdvertisementGetBannerWidth", &Detail::appleAdvertisement_getBannerWidth );
        pybind::def_function_kernel( _kernel, "appleAdvertisementGetBannerHeight", &Detail::appleAdvertisement_getBannerHeight );
        
        pybind::def_function( _kernel, "appleAdvertisementCanYouShowInterstitial", &Detail::appleAdvertisement_canYouShowInterstitial );
        pybind::def_function( _kernel, "appleAdvertisementShowInterstitial", &Detail::appleAdvertisement_showInterstitial );
        
        pybind::def_function( _kernel, "appleAdvertisementCanOfferRewarded", &Detail::appleAdvertisement_canOfferRewarded );
        pybind::def_function( _kernel, "appleAdvertisementCanYouShowRewarded", &Detail::appleAdvertisement_canYouShowRewarded );
        pybind::def_function( _kernel, "appleAdvertisementShowRewarded", &Detail::appleAdvertisement_showRewarded );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAdvertisementSetAdvertisementCallback", nullptr );
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

