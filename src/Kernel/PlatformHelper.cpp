#include "PlatformHelper.h"

#include "Interface/PlatformServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event )
        {
            PLATFORM_SERVICE()
                ->dispatchMainThreadEvent( _event );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}