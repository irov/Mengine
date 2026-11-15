#include "Win32OpButtonLoggerListener.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonLoggerListener::Win32OpButtonLoggerListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonLoggerListener::~Win32OpButtonLoggerListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonLoggerListener::onOpButtonLoggerMessage( OpButton::OpButtonLogLevel _level, const OpButton::OpButtonString & _tag, const OpButton::OpButtonString & _message )
    {
        switch( _level )
        {
        case OpButton::OpButtonLogLevel::PL_DEBUG:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_DEBUG, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        case OpButton::OpButtonLogLevel::PL_INFO:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_INFO, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        case OpButton::OpButtonLogLevel::PL_WARNING:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_WARNING, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        case OpButton::OpButtonLogLevel::PL_ERROR:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_ERROR, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        case OpButton::OpButtonLogLevel::PL_CRITICAL:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_FATAL, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        case OpButton::OpButtonLogLevel::PL_EXCEPTION:
            {
                LOGGER_VERBOSE_LEVEL( "opbutton", LM_FATAL, LFILTER_NONE, LCOLOR_NONE, nullptr, 0, nullptr, LFLAG_SHORT )( "[%s] %s", _tag.c_str(), _message.c_str() );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
