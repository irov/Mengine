#include "iOSAdjustScriptEmbedding.h"

#import "iOSAdjustPlugin.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdjust_eventTraking( NSString * _token )
        {
            iOSAdjustPlugin * delegate = [iOSAdjustPlugin sharedInstance];

            if( delegate == nil )
            {
                return false;
            }

            [delegate eventTraking:_token];

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAdjust_revenueTracking( NSString * _token, double _amount, NSString * _currency )
        {
            iOSAdjustPlugin * delegate = [iOSAdjustPlugin sharedInstance];

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
    iOSAdjustScriptEmbedding::iOSAdjustScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAdjustScriptEmbedding::~iOSAdjustScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSAdjustScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "appleAdjustEventTraking", &Detail::iOSAdjust_eventTraking );
        pybind::def_function( _kernel, "appleAdjustRevenueTracking",
                              &Detail::iOSAdjust_revenueTracking );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAdjustScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAdjustEventTraking", nullptr );
        _kernel->remove_from_module( "appleAdjustRevenueTracking", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
