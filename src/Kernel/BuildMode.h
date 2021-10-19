#pragma once

#include "Config/Config.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool isDebugMode();
        bool isDevelopmentMode();
        bool isQualityAssuranceMode();
        bool isBuildPublish();
        bool isBuildMaster();
        bool isWindowsDebug();
        const Char * getEngineGITSHA1();
        const Char * getBuildTimestamp();
        const Char * getBuildUsername();
        const Char * getBuildVersion();
    }
}