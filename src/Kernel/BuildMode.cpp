#include "BuildMode.h"

#include "Interface/OptionsServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isDevelopmentMode()
        {
            if( SERVICE_IS_INITIALIZE( OptionsServiceInterface ) == true )
            {
                if( HAS_OPTION( "dev" ) == true )
                {
                    return true;
                }
            }

#ifdef MENGINE_BUILD_DEVELOPMENT
            return true;
#else
            return false;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        bool isBuildPublish()
        {
            return MENGINE_PUBLISH_VALUE( true, false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isBuildMaster()
        {
            return MENGINE_MASTER_VALUE( true, false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isWindowsDebug()
        {
            return MENGINE_WINDOWS_DEBUG_VALUE( true, false );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}