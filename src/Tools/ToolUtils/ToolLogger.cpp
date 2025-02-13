#include "ToolLogger.h"
#include "ToolUtils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ToolLogger::ToolLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolLogger::~ToolLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        ::fprintf( stdout, "%s:%d %.*s\n"
            , message.function
            , message.line
            , (int)message.size
            , message.data
        );

        ::fflush( stdout );
    };
    //////////////////////////////////////////////////////////////////////////
}