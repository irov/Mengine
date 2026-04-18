#include "Win32PilotLoggerListener.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotLoggerListener::onPilotLoggerMessage( Pilot::PilotLogLevel level, const Pilot::PilotString & tag, const Pilot::PilotString & message )
    {
        switch( level )
        {
        case Pilot::PilotLogLevel::PL_DEBUG:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_DEBUG, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        case Pilot::PilotLogLevel::PL_INFO:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_INFO, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        case Pilot::PilotLogLevel::PL_WARNING:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_WARNING, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        case Pilot::PilotLogLevel::PL_ERROR:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_ERROR, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        case Pilot::PilotLogLevel::PL_CRITICAL:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_FATAL, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        case Pilot::PilotLogLevel::PL_EXCEPTION:
            {
                LOGGER_VERBOSE_LEVEL( "pilot", LM_FATAL, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s", tag.c_str(), message.c_str());
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
