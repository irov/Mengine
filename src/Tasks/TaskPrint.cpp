#include "TaskPrint.h"

#include "Kernel/Logger.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskPrint::TaskPrint( const Char * _format, ... )
    {
        va_list args;
        va_start( args, _format );

        Char message[2048] = { 0 };
        int message_size = MENGINE_VSNPRINTF( message, 2047, _format, args );

        m_message.assign( message, message_size );

        va_end( args );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPrint::~TaskPrint()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPrint::_onRun()
    {
        LOGGER_MESSAGE( "%s"
            , m_message.c_str()
        );

        return true;
    }
}