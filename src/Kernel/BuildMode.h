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
        const Char * getEngineGITSHA1();
        const Char * getEngineGITURL();
        const Char * getEngineGITBranch();
        const Char * getEngineGITDate();
        const Char * getEngineVersion();
        const Char * getContentCommit();
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