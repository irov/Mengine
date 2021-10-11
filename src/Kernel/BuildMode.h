#pragma once

namespace Mengine
{
    namespace Helper
    {
        bool isDevelopmentMode();
        bool isQualityAssuranceMode();
        bool isBuildPublish();
        bool isBuildMaster();
        bool isWindowsDebug();
    }
}