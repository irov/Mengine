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
        static void iOSAdvertisement_setBannerCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementBannerCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setBannerCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAdvertisement_setInterstitialCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementInterstitialCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setInterstitialCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAdvertisement_setRewardedCallback( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAdvertisementCallbackInterface> callback = [[PythoniOSAdvertisementRewardedCallback alloc] initWithCbs:_cbs args:_args];

            [[iOSAdvertisementPlugin sharedInstance] setRewardedCallback:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_hasBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] hasBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_showBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] showBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_hideBanner()
        {
            if ([[iOSAdvertisementPlugin sharedInstance] hideBanner] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * iOSAdvertisement_getBannerWidth( pybind::kernel_interface * _kernel )
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
        static PyObject * iOSAdvertisement_getBannerHeight( pybind::kernel_interface * _kernel )
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
        static bool iOSAdvertisement_hasInterstitial() {
            if ([[iOSAdvertisementPlugin sharedInstance] hasInterstitial] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_canYouShowInterstitial( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canYouShowInterstitial:_placement] == NO) {
                return false;
            }
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_showInterstitial( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] showInterstitial:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_isShowingInterstitial() {
            if ([[iOSAdvertisementPlugin sharedInstance] isShowingInterstitial] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_hasRewarded() {
            if ([[iOSAdvertisementPlugin sharedInstance] hasRewarded] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_canOfferRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canOfferRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_canYouShowRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] canYouShowRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_showRewarded( NSString * _placement ) {
            if ([[iOSAdvertisementPlugin sharedInstance] showRewarded:_placement] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_isShowingRewarded() {
            if ([[iOSAdvertisementPlugin sharedInstance] isShowingRewarded] == NO) {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdvertisement_getNoAds() {
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
        pybind::def_function_args( _kernel, "iOSAdvertisementSetBannerCallback", &Detail::iOSAdvertisement_setBannerCallback );
        pybind::def_function_args( _kernel, "iOSAdvertisementSetInterstitialCallback", &Detail::iOSAdvertisement_setInterstitialCallback );
        pybind::def_function_args( _kernel, "iOSAdvertisementSetRewardedCallback", &Detail::iOSAdvertisement_setRewardedCallback );

        pybind::def_function( _kernel, "iOSAdvertisementHasBanner", &Detail::iOSAdvertisement_hasBanner );
        pybind::def_function( _kernel, "iOSAdvertisementShowBanner", &Detail::iOSAdvertisement_showBanner );
        pybind::def_function( _kernel, "iOSAdvertisementHideBanner", &Detail::iOSAdvertisement_hideBanner );
        pybind::def_function_kernel( _kernel, "iOSAdvertisementGetBannerWidth", &Detail::iOSAdvertisement_getBannerWidth );
        pybind::def_function_kernel( _kernel, "iOSAdvertisementGetBannerHeight", &Detail::iOSAdvertisement_getBannerHeight );

        pybind::def_function( _kernel, "iOSAdvertisementHasInterstitial", &Detail::iOSAdvertisement_hasInterstitial );
        pybind::def_function( _kernel, "iOSAdvertisementCanYouShowInterstitial", &Detail::iOSAdvertisement_canYouShowInterstitial );
        pybind::def_function( _kernel, "iOSAdvertisementShowInterstitial", &Detail::iOSAdvertisement_showInterstitial );
        pybind::def_function( _kernel, "iOSAdvertisementIsShowingInterstitial", &Detail::iOSAdvertisement_isShowingInterstitial );

        pybind::def_function( _kernel, "iOSAdvertisementHasRewarded", &Detail::iOSAdvertisement_hasRewarded );
        pybind::def_function( _kernel, "iOSAdvertisementCanOfferRewarded", &Detail::iOSAdvertisement_canOfferRewarded );
        pybind::def_function( _kernel, "iOSAdvertisementCanYouShowRewarded", &Detail::iOSAdvertisement_canYouShowRewarded );
        pybind::def_function( _kernel, "iOSAdvertisementShowRewarded", &Detail::iOSAdvertisement_showRewarded );
        pybind::def_function( _kernel, "iOSAdvertisementIsShowingRewarded", &Detail::iOSAdvertisement_isShowingRewarded );

        pybind::def_function( _kernel, "iOSAdvertisementGetNoAds", &Detail::iOSAdvertisement_getNoAds );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAdvertisementScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSAdvertisementSetBannerCallback", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementSetInterstitialCallback", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementSetRewardedCallback", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementHasBanner", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementShowBanner", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementHideBanner", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementGetBannerWidth", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementGetBannerHeight", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementHasInterstitial", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementCanYouShowInterstitial", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementShowInterstitial", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementIsShowingInterstitial", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementHasRewarded", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementShowRewarded", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementIsShowingRewarded", nullptr );
        _kernel->remove_from_module( "iOSAdvertisementGetNoAds", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
