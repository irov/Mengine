#include "iOSProxyLogger.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/AssertionUtf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSProxyLogger::iOSProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSProxyLogger::~iOSProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSProxyLogger::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSProxyLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSProxyLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );
        
        iOSLogRecordParam * record = [iOSLogRecordParam alloc];
        
        record.LOG_TIMESTAMP = message.timestamp;
        record.LOG_CATEGORY = Helper::stringToNSString( message.category );
        record.LOG_THREADNAME = Helper::stringToNSString( message.threadName );
        record.LOG_LEVEL = message.level;
        record.LOG_FLAG = message.flag;
        record.LOG_FILTER = message.filter;
        record.LOG_COLOR = message.color;
        record.LOG_FUNCTION = Helper::stringToNSString( message.function );
        record.LOG_LINE = message.line;
        record.LOG_DATA = Helper::stringToNSString( message.data, message.size );
        
        Helper::iOSLog( record );
    }
    //////////////////////////////////////////////////////////////////////////
}
