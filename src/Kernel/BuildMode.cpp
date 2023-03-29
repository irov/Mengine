#include "BuildMode.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/GitSHA1.h"
#include "Config/BuildInfo.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isDebugMode()
        {
            bool debugMode = MENGINE_DEBUG_VALUE( true, false );
            
            return debugMode;
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

#if defined(MENGINE_BUILD_DEVELOPMENT)
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
            bool buildPublish = MENGINE_BUILD_PUBLISH_VALUE( true, false );

            return buildPublish;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isMasterRelease()
        {
            bool masterRelease = MENGINE_MASTER_RELEASE_VALUE( true, false );

            return masterRelease;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isWindowsDebug()
        {
            bool windowDebug = MENGINE_WINDOWS_DEBUG_VALUE( true, false );

            return windowDebug;
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
        const Char * getEngineGITDate()
        {
            const Char * ENGINE_GIT_DATE = MENGINE_ENGINE_GIT_DATE;

            return ENGINE_GIT_DATE;
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
        const Char * getBuildProjectName()
        {
            const Char * BUILD_PROJECT_NAME = MENGINE_BUILD_PROJECT_NAME;

            return BUILD_PROJECT_NAME;
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
        uint64_t getBuildNumber()
        {
            uint64_t BUILD_NUMBER = MENGINE_BUILD_NUMBER;

            return BUILD_NUMBER;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}