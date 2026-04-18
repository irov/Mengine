#include "Win32PilotLogger.h"

#include "Kernel/LoggerMessage.h"

#include "Pilot/PilotSdk.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PilotLogger::Win32PilotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PilotLogger::~Win32PilotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        Pilot::PilotLogLevel level;

        switch( message.level )
        {
        case LM_FATAL:
            {
                level = Pilot::PilotLogLevel::PL_CRITICAL;
            }break;
        case LM_ERROR:
            {
                level = Pilot::PilotLogLevel::PL_ERROR;
            }break;
        case LM_WARNING:
            {
                level = Pilot::PilotLogLevel::PL_WARNING;
            }break;
        case LM_MESSAGE_RELEASE:
        case LM_MESSAGE:
        case LM_INFO:
            {
                level = Pilot::PilotLogLevel::PL_INFO;
            }break;
        case LM_DEBUG:
        case LM_VERBOSE:
            {
                level = Pilot::PilotLogLevel::PL_DEBUG;
            }break;
        default:
            return;
        }

        Pilot::PilotString msg( message.data, message.size );
        Pilot::PilotString category = message.category != nullptr ? Pilot::PilotString( message.category ) : Pilot::PilotString();
        Pilot::PilotString thread;

        const ConstString & threadName = message.thread;

        if( threadName.empty() == false )
        {
            thread = threadName.c_str();
        }

        Pilot::PilotSdk::log( level, msg, category, thread );
    }
    //////////////////////////////////////////////////////////////////////////
}
