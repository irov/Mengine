#include "AppleAdjustScriptEmbedding.h"

#include "AppleAdjustInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleAdjust_eventTraking( const ConstString & _token )
        {
            APPLE_ADJUST_SERVICE()
                ->eventTraking( _token );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_AppleAdjust_revenueTracking( const ConstString & _token, double _amount, const ConstString & _currency )
        {
            APPLE_ADJUST_SERVICE()
                ->revenueTracking( _token, _amount, _currency );
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleAdjust", true );

        pybind::def_function( _kernel, "appleAdjustEventTraking", &Detail::s_AppleAdjust_eventTraking );
        pybind::def_function( _kernel, "appleAdjustRevenueTracking", &Detail::s_AppleAdjust_revenueTracking );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdjustScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleMARSDKSetProvider", nullptr );
        _kernel->remove_from_module( "appleMARSDKLogin", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

