#include "iOSAdvertisementScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#import "Environment/Python/ApplePythonProvider.h"

#import "iOSAdvertisementPlugin.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"


@interface PythoniOSAdvertisementBannerCallback : ApplePythonProvider<iOSAdvertisementCallbackInterface>
@end

@implementation PythoniOSAdvertisementBannerCallback

- (void)oniOSAdvertisementShowSuccess:(NSString *)placement {
    //Empty
}

- (void)oniOSAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    //Empty
}

- (void)oniOSAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementRevenuePaid"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    //Empty
}

@end

@interface PythoniOSAdvertisementInterstitialCallback : ApplePythonProvider<iOSAdvertisementCallbackInterface>
@end

@implementation PythoniOSAdvertisementInterstitialCallback

- (void)oniOSAdvertisementShowSuccess:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementShowSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    NSDictionary * params = @{
        @"placement": placement,
        @"errorCode": @(code)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementShowFailed"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementRevenuePaid"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    //Empty
}

@end

@interface PythoniOSAdvertisementRewardedCallback : ApplePythonProvider<iOSAdvertisementCallbackInterface>
@end

@implementation PythoniOSAdvertisementRewardedCallback

- (void)oniOSAdvertisementShowSuccess:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementShowSuccess"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementShowFailed:(NSString *)placement withError:(NSInteger)code {
    NSDictionary * params = @{
        @"placement": placement,
        @"errorCode": @(code)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementShowFailed"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementRevenuePaid:(NSString *)placement withRevenue:(double)revenue {
    NSDictionary * params = @{
        @"placement": placement,
        @"revenue": @(revenue)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementRevenuePaid"];

    if( py_cb.is_callable() == false )
    {
        return;
    }

    py_cb.call_args( params, self.m_args );
}

- (void)oniOSAdvertisementUserRewarded:(NSString *)placement withLabel:(NSString *)label withAmount:(NSInteger)amount {
    NSDictionary * params = @{
        @"placement": placement,
        @"label": label,
        @"amount": @(amount)
    };

    pybind::object py_cb = [self getMethod:@"oniOSAdvertisementUserRewarded"];

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
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementBannerCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setBannerCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setInterstitialCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementInterstitialCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setInterstitialCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAdvertisement_setRewardedCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementRewardedCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setRewardedCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] hasBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] showBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hideBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] hideBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * appleAdvertisement_getBannerWidth( pybind::kernel_interface * _kernel )
        {
            uint32_t width;
            uint32_t height;
            if ([[iOSAdvertisementPlugin sharedInstance] getBannerWidth:&width height:&height] == NO) {
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
            if ([[iOSAdvertisementPlugin sharedInstance] getBannerWidth:&width height:&height] == NO) {
                return _kernel->ret_none();
            }

            PyObject * py_height = pybind::ptr( _kernel, height );

            return py_height;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasInterstitial() {
            if ([[iOSAdvertisementPlugin sharedInstance] hasInterstitial] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canYouShowInterstitial( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canYouShowInterstitial:_placement] == NO) {
                return false;
            }
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showInterstitial( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] showInterstitial:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_isShowingInterstitial() {
            if ([[iOSAdvertisementPlugin sharedInstance] isShowingInterstitial] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_hasRewarded() {
            if ([[iOSAdvertisementPlugin sharedInstance] hasRewarded] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canOfferRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canOfferRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_canYouShowRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canYouShowRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_showRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] showRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_isShowingRewarded() {
            if ([[iOSAdvertisementPlugin sharedInstance] isShowingRewarded] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAdvertisement_getNoAds() {
            if ([[iOSAdvertisementPlugin sharedInstance] getNoAds] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAdvertisementScriptEmbedding::iOSAdvertisementScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAdvertisementScriptEmbedding::~iOSAdvertisementScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSAdvertisementScriptEmbedding::embed( pybind::kernel_interface * _kernel )
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
        pybind::def_function( _kernel, "appleAdvertisementIsShowingInterstitial", &Detail::appleAdvertisement_isShowingInterstitial );

        pybind::def_function( _kernel, "appleAdvertisementHasRewarded", &Detail::appleAdvertisement_hasRewarded );
        pybind::def_function( _kernel, "appleAdvertisementCanOfferRewarded", &Detail::appleAdvertisement_canOfferRewarded );
        pybind::def_function( _kernel, "appleAdvertisementCanYouShowRewarded", &Detail::appleAdvertisement_canYouShowRewarded );
        pybind::def_function( _kernel, "appleAdvertisementShowRewarded", &Detail::appleAdvertisement_showRewarded );
        pybind::def_function( _kernel, "appleAdvertisementIsShowingRewarded", &Detail::appleAdvertisement_isShowingRewarded );

        pybind::def_function( _kernel, "appleAdvertisementGetNoAds", &Detail::appleAdvertisement_getNoAds );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAdvertisementScriptEmbedding::eject( pybind::kernel_interface * _kernel )
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
        _kernel->remove_from_module( "appleAdvertisementIsShowingInterstitial", nullptr );
        _kernel->remove_from_module( "appleAdvertisementCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementIsShowingRewarded", nullptr );
        _kernel->remove_from_module( "appleAdvertisementGetNoAds", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
