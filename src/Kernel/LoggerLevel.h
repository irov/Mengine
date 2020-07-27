#pragma once

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ELoggerLevel
    {
        LM_FATAL = 0,
        LM_CRITICAL,
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
}