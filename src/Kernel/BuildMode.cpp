#include "BuildMode.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/ConfigHelper.h"

#include "Config/GitSHA1.h"
#include "Config/BuildInfo.h"
#include "Config/BuildVersion.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isDebugMode()
        {
            return MENGINE_DEBUG_VALUE( true, false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isDevelopmentMode()
        {
            if( SERVICE_IS_INITIALIZE( OptionsServiceInterface ) == true )
            {
                if( HAS_OPTION( "nodev" ) == true )
                {
                    return false;
                }

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
        bool isQualityAssuranceMode()
        {
            if( SERVICE_IS_INITIALIZE( OptionsServiceInterface ) == true )
            {
                if( HAS_OPTION( "qa" ) == true )
                {
                    return true;
                }
            }

            return false;
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
        const Char * getEngineGITSHA1()
        {
            const Char * ENGINE_GIT_SHA1 = MENGINE_ENGINE_GIT_SHA1;

            return ENGINE_GIT_SHA1;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineGITURL()
        {
            const Char * ENGINE_GIT_URL = MENGINE_ENGINE_GIT_URL;

            return ENGINE_GIT_URL;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineGITBranch()
        {
            const Char * ENGINE_GIT_BRANCH = MENGINE_ENGINE_GIT_BRANCH;

            return ENGINE_GIT_BRANCH;
        }
        //////////////////////////////////////////////////////////////////////////        
        const Char * getContentCommit()
        {
            const Char * defaultContentCommit = "0000000000000000000000000000000000000000";

            if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == true )
            {
                const Char * Info_ContentCommit = Mengine::Detail::getConfigValue( "Info", "ContentCommit", defaultContentCommit );

                return Info_ContentCommit;
            }

            return defaultContentCommit;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildTimestamp()
        {
            const Char * BUILD_TIMESTAMP = MENGINE_BUILD_TIMESTAMP;

            return BUILD_TIMESTAMP;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildUsername()
        {
            const Char * BUILD_USERNAME = MENGINE_BUILD_USERNAME;

            return BUILD_USERNAME;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildVersion()
        {
            const Char * BUILD_VERSION = MENGINE_BUILD_VERSION;

            return BUILD_VERSION;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}