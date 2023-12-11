#include "PlatformHelper.h"

#include "Interface/PlatformServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event )
        {
            if( SERVICE_IS_INITIALIZE( PlatformServiceInterface ) == true )
            {
                PLATFORM_SERVICE()
                    ->dispatchMainThreadEvent( _event );
            };
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
