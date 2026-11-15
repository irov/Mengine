#include "Win32OpButtonLogger.h"

#include "Kernel/LoggerMessage.h"

#include "Config/StdString.h"

#include "OpButton/OpButtonSdk.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonLogger::Win32OpButtonLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonLogger::~Win32OpButtonLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        const Char * messageCategory = message.category;

        if( messageCategory != nullptr )
        {
            if( StdString::strcmp( messageCategory, "opbutton" ) == 0 )
            {
                return;
            }
        }

        OpButton::OpButtonLogLevel level;

        switch( message.level )
        {
        case LM_FATAL:
            {
                level = OpButton::OpButtonLogLevel::PL_CRITICAL;
            }break;
        case LM_ERROR:
            {
                level = OpButton::OpButtonLogLevel::PL_ERROR;
            }break;
        case LM_WARNING:
            {
                level = OpButton::OpButtonLogLevel::PL_WARNING;
            }break;
        case LM_MESSAGE_RELEASE:
        case LM_MESSAGE:
        case LM_INFO:
            {
                level = OpButton::OpButtonLogLevel::PL_INFO;
            }break;
        case LM_DEBUG:
        case LM_VERBOSE:
            {
                level = OpButton::OpButtonLogLevel::PL_DEBUG;
            }break;
        default:
            return;
        }

        OpButton::OpButtonString msg( message.data, message.size );
        OpButton::OpButtonString category = messageCategory != nullptr ? OpButton::OpButtonString( messageCategory ) : OpButton::OpButtonString();
        OpButton::OpButtonString thread;

        const ConstString & threadName = message.thread;

        if( threadName.empty() == false )
        {
            thread = threadName.c_str();
        }

        OpButton::OpButtonSdk::log( level, msg, category, thread );
    }
    //////////////////////////////////////////////////////////////////////////
}
