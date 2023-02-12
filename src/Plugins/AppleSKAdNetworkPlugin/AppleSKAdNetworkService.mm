#include "AppleSKAdNetworkService.h"

#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/StdIntTypes.h"

#import <StoreKit/SKAdNetwork.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleSKAdNetworkService, Mengine::AppleSKAdNetworkService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkService::AppleSKAdNetworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkService::~AppleSKAdNetworkService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSKAdNetworkService::_initializeService()
    {
        //Empty

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleSKAdNetworkService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSKAdNetworkService::updateConversionValue( int64_t _value, const LambdaConversionValue & _lambda )
    {
        NSInteger value_ns = (NSInteger)_value;
        
        if (@available(iOS 15.4, *)) {
            [SKAdNetwork updatePostbackConversionValue:value_ns completionHandler:^(NSError * _Nullable error) {
                if( error != nil )
                {
                    LOGGER_ERROR("updatePostbackConversionValue value: %" MENGINE_PRId64 " error: %s"
                                 , _value
                                 , Helper::AppleGetMessageFromNSError( error ).c_str()
                                 );
                    
                    _lambda( false );
                    
                    return;
                }
                
                _lambda( true );
            }];
        } else if (@available(iOS 14.0, *)) {
            [SKAdNetwork updateConversionValue:value_ns];
            
            _lambda( true );
        } else {
            _lambda( true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
