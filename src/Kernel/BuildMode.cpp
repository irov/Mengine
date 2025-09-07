#include "BuildMode.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/Version.h"

#include "Configuration/GitSHA1.h"
#include "Configuration/BuildInfo.h"

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
        bool isValidationMode()
        {
            if( SERVICE_IS_INITIALIZE( OptionsServiceInterface ) == true )
            {
                if( HAS_OPTION( "validation" ) == true )
                {
                    return true;
                }
            }

            return false;
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
        const Char * getEngineGitSHA1()
        {
            const Char * ENGINE_GIT_SHA1 = MENGINE_ENGINE_GIT_SHA1;

            return ENGINE_GIT_SHA1;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineGitURL()
        {
            const Char * ENGINE_GIT_URL = MENGINE_ENGINE_GIT_URL;

            return ENGINE_GIT_URL;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineGitBranch()
        {
            const Char * ENGINE_GIT_BRANCH = MENGINE_ENGINE_GIT_BRANCH;

            return ENGINE_GIT_BRANCH;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineGitDate()
        {
            const Char * ENGINE_GIT_DATE = MENGINE_ENGINE_GIT_DATE;

            return ENGINE_GIT_DATE;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getEngineVersion()
        {
            const Char * ENGINE_VERSION = MENGINE_ENGINE_VERSION_STRING;

            return ENGINE_VERSION;
        }
        //////////////////////////////////////////////////////////////////////////        
        const Char * getContentCommit()
        {
            const Char * defaultContentCommit = "0000000000000000000000000000000000000000";

            if( SERVICE_IS_INITIALIZE( ConfigServiceInterface ) == true )
            {
                const Char * Info_ContentCommit = CONFIG_VALUE_STRING( "Info", "ContentCommit", defaultContentCommit );

                return Info_ContentCommit;
            }

            return defaultContentCommit;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildSolutionName()
        {
            const Char * BUILD_SOLUTION_NAME = MENGINE_BUILD_SOLUTION_NAME;

            return BUILD_SOLUTION_NAME;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildDate()
        {
            const Char * BUILD_DATE = MENGINE_BUILD_DATE;

            return BUILD_DATE;
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
        uint32_t getBuildVersionMajor()
        {
            uint32_t buildVersionMajor = MENGINE_BUILD_VERSION_MAJOR;

            return buildVersionMajor;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getBuildVersionMinor()
        {
            uint32_t buildVersionMinor = MENGINE_BUILD_VERSION_MINOR;

            return buildVersionMinor;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getBuildVersionPatch()
        {
            uint32_t buildVersionPatch = MENGINE_BUILD_VERSION_PATCH;

            return buildVersionPatch;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getBuildVersionNumber()
        {
            uint32_t buildVersionNumber = MENGINE_BUILD_VERSION_NUMBER;

            return buildVersionNumber;
        }
        //////////////////////////////////////////////////////////////////////////
        uint64_t getBuildNumber()
        {
            uint64_t BUILD_NUMBER = MENGINE_BUILD_NUMBER;

            return BUILD_NUMBER;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getBuildNumberString()
        {
            const Char * BUILD_NUMBER_STRING = MENGINE_BUILD_NUMBER_STRING;

            return BUILD_NUMBER_STRING;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}