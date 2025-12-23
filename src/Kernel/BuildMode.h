#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool isDebugMode();
        bool isDevelopmentMode();
        bool isValidationMode();
        bool isQualityAssuranceMode();
        bool isBuildPublish();
        bool isMasterRelease();
        bool isWindowsDebug();
        const Char * getEngineGitSHA1();
        const Char * getEngineGitURL();
        const Char * getEngineGitBranch();
        const Char * getEngineGitDate();
        const Char * getEngineVersion();
        const Char * getBuildSolutionName();
        const Char * getBuildDate();
        const Char * getBuildUsername();
        const Char * getBuildVersion();
        uint32_t getBuildVersionMajor();
        uint32_t getBuildVersionMinor();
        uint32_t getBuildVersionPatch();
        uint32_t getBuildVersionNumber();
        uint64_t getBuildNumber();
        const Char * getBuildNumberString();
    }
}