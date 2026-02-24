#include "AppleAdjustScriptEmbedding.h"

#import "AppleAdjustApplicationDelegate.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool AppleAdjust_eventTraking( NSString * _token )
        {
            AppleAdjustApplicationDelegate * delegate = [AppleAdjustApplicationDelegate sharedInstance];

            if( delegate == nil )
            {
                return false;
            }

            [delegate eventTraking:_token];

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleAdjust_revenueTracking( NSString * _token, double _amount, NSString * _currency )
        {
            AppleAdjustApplicationDelegate * delegate = [AppleAdjustApplicationDelegate sharedInstance];

            if( delegate == nil )
            {
                return false;
            }

            [delegate revenueTracking:_token amount:_amount currency:_currency];

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustScriptEmbedding::AppleAdjustScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustScriptEmbedding::~AppleAdjustScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdjustScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "appleAdjustEventTraking", &Detail::AppleAdjust_eventTraking );
        pybind::def_function( _kernel, "appleAdjustRevenueTracking",
                              &Detail::AppleAdjust_revenueTracking );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdjustScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAdjustEventTraking", nullptr );
        _kernel->remove_from_module( "appleAdjustRevenueTracking", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

