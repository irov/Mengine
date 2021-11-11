#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerLevel
    {
        LM_SILENT = 0,
        LM_FATAL,
        LM_CRITICAL,
        LM_MESSAGE_RELEASE,
        LM_ERROR,
        LM_PERFOMANCE,
        LM_STATISTIC,
        LM_WARNING,
        LM_MESSAGE,
        LM_INFO,
        LM_DEBUG,
        LM_VERBOSE,

        __LM_MAX__
    };
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerColor
    {
        LCOLOR_NONE = 1 << 0,
        LCOLOR_RED = 1 << 1,
        LCOLOR_BLUE = 1 << 2,
        LCOLOR_GREEN = 1 << 3
    };
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerFilter
    {
        LFILTER_NONE = 0 << 0,
        LFILTER_PROTECTED = 1 << 0,
    };
    //////////////////////////////////////////////////////////////////////////
}