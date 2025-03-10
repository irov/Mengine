#include "AppleAdvertisementScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "AppleAdvertisementApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"


@interface PythonAppleAdvertisementResponse : NSObject<AppleAdvertisementResponseInterface>
- (instancetype)initWithCbs:(const pybind::dict &)cbs args:(const pybind::args &)args;

@property (nonatomic, assign) pybind::dict m_cbs;
@property (nonatomic, assign) pybind::args m_args;
@end

@implementation PythonAppleAdvertisementResponse

- (instancetype)initWithCbs:(const pybind::dict &)cbs args:(const pybind::args &)args {
    self = [super init];
    
    self.m_cbs = cbs;
    self.m_args = args;
    
    return self;
}

- (void)onAppleAdvertisementShowSuccess:(iOSAdFormat *)format withPlacement:(NSString *)placement {
    NSDictionary * params = @{
        @"placement": placement
    };
    
    if (format == iOSAdFormat.ADFORMAT_INTERSTITIAL) {
        pybind::object py_cb = self.m_cbs["onAppleAppLovinInterstitialShowSuccess"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = self.m_cbs["onAppleAppLovinRewardedShowSuccess"];
        
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
        pybind::object py_cb = self.m_cbs["onAppleAppLovinInterstitialShowFailed"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = self.m_cbs["onAppleAppLovinRewardedShowFailed"];
        
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
        pybind::object py_cb = pybind::dict( self.m_cbs )["onAppleAppLovinBannerRevenuePaid"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_INTERSTITIAL) {
        pybind::object py_cb = pybind::dict( self.m_cbs )["onAppleAppLovinInterstitialRevenuePaid"];
        
        if( py_cb.is_callable() == false )
        {
            return;
        }
        
        py_cb.call_args( params, self.m_args );
    } else if (format == iOSAdFormat.ADFORMAT_REWARDED) {
        pybind::object py_cb = pybind::dict( self.m_cbs )["onAppleAppLovinRewardedRevenuePaid"];
        
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
        pybind::object py_cb = pybind::dict( self.m_cbs )["onAppleAppLovinRewardedUserRewarded"];
        
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
        static void appleAppLovin_setAdvertisementResponse( const pybind::dict & _cbs, const pybind::args & _args )
        {
            PythonAppleAdvertisementResponse * response = [[PythonAppleAdvertisementResponse alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAdvertisementApplicationDelegate sharedInstance] setAdvertisementResponse:response];
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * appleAppLovin_getBannerHeight( pybind::kernel_interface * _kernel )
        {
            uint32_t width;
            uint32_t height;
            if ([[AppleAdvertisementApplicationDelegate sharedInstance] getBannerSize:&width height:&height] == NO) {
                return _kernel->ret_none();
            }
            
            PyObject * py_viewport = pybind::ptr( _kernel, height );
            
            return py_viewport;
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
        
        AppleAppLovinServiceInterface * service = APPLE_APPLOVIN_SERVICE();
        
        pybind::def_function_args( _kernel, "appleAppLovinSetBannerProvider", &Detail::appleAppLovin_setBannerProvider );
        pybind::def_function_args( _kernel, "appleAppLovinSetInterstitialProvider", &Detail::appleAppLovin_setInterstitialProvider );
        pybind::def_function_args( _kernel, "appleAppLovinSetRewardedProvider", &Detail::appleAppLovin_setRewardedProvider );
        
        pybind::def_functor( _kernel, "appleAppLovinShowBanner", service, &AppleAppLovinServiceInterface::showBanner );
        pybind::def_functor( _kernel, "appleAppLovinHideBanner", service, &AppleAppLovinServiceInterface::hideBanner );
        pybind::def_function_kernel( _kernel, "appleAppLovinGetBannerHeight", &Detail::appleAppLovin_getBannerHeight );
        
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowInterstitial", service,  &AppleAppLovinServiceInterface::canYouShowInterstitial );
        pybind::def_functor( _kernel, "appleAppLovinShowInterstitial", service, &AppleAppLovinServiceInterface::showInterstitial );
        
        pybind::def_functor( _kernel, "appleAppLovinCanOfferRewarded", service, &AppleAppLovinServiceInterface::canOfferRewarded );
        pybind::def_functor( _kernel, "appleAppLovinCanYouShowRewarded", service, &AppleAppLovinServiceInterface::canYouShowRewarded );
        pybind::def_functor( _kernel, "appleAppLovinShowRewarded", service, &AppleAppLovinServiceInterface::showRewarded );
        
        pybind::def_functor( _kernel, "appleAppLovinHasSupportedCMP", service, &AppleAppLovinServiceInterface::hasSupportedCMP );
        pybind::def_functor( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", service, &AppleAppLovinServiceInterface::isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::appleAppLovin_loadAndShowCMPFlow );
        pybind::def_functor( _kernel, "appleAppLovinShowMediationDebugger", service, &AppleAppLovinServiceInterface::showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinSetProvider", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinHideBanner", nullptr );
        _kernel->remove_from_module( "appleAppLovinGetBannerViewport", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowInterstitial", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanOfferRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinCanYouShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowRewarded", nullptr );
        _kernel->remove_from_module( "appleAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

