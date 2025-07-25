#include "iOSLogger.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

#define MENGINE_APPLE_LOG_MAX_MESSAGE 1024

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSLogger::iOSLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSLogger::~iOSLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );
        
        AppleLogRecordParam * record = [AppleLogRecordParam alloc];
        
        record.LOG_TIMESTAMP = message.timestamp;
        record.LOG_CATEGORY = [NSString stringWithUTF8String:message.category];
        record.LOG_THREAD = [AppleString NSStringFromConstString:message.thread];
        record.LOG_LEVEL = message.level;
        record.LOG_FLAG = message.flag;
        record.LOG_FILTER = message.filter;
        record.LOG_COLOR = message.color;
        record.LOG_FILE = (message.file != nullptr) ? [NSString stringWithUTF8String:message.file] : nil;
        record.LOG_LINE = message.line;
        record.LOG_FUNCTION = (message.function != nullptr) ? [NSString stringWithUTF8String:message.function] : nil;
        record.LOG_DATA = [AppleString NSStringFromString:message.data withSize:message.size];
        
        [iOSDetail log:record];
    }
    //////////////////////////////////////////////////////////////////////////
}
