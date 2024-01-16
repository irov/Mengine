#include "PlatformHelper.h"

#include "Interface/ThreadServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event )
        {
            if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == true )
            {
                THREAD_SERVICE()
                    ->dispatchMainThreadEvent( _event );
            };
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
