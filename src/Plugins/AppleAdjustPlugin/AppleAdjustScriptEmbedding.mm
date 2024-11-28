#include "AppleAdjustScriptEmbedding.h"

#include "AppleAdjustInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool AppleAdjust_eventTraking(const ConstString & _token )
        {
            APPLE_ADJUST_SERVICE()
                ->eventTraking( _token );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleAdjust_revenueTracking(const ConstString & _token, double _amount, const ConstString & _currency )
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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleAdjust"), true );

        pybind::def_function( _kernel, "appleAdjustEventTraking", &Detail::AppleAdjust_eventTraking );
        pybind::def_function( _kernel, "appleAdjustRevenueTracking",
                              &Detail::AppleAdjust_revenueTracking );

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

