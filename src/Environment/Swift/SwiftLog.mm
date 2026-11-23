#import "SwiftLog.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLogRecordParam.h"
#import "Environment/iOS/iOSDetail.h"

#include "Kernel/Assertion.h"

void MengineSwiftLogWrite( ESwiftLogLevel _level, NSString * (NS_NOESCAPE ^_category)(void), NSString * _file, int32_t _line, NSString * _function, NSString * (NS_NOESCAPE ^_message)(void) )
{
    Mengine::ELoggerLevel level;
    uint32_t color;

    switch( _level )
    {
    case ESwiftLogLevelError:
        level = Mengine::LM_ERROR;
        color = Mengine::LCOLOR_RED;
        break;
    case ESwiftLogLevelWarning:
        level = Mengine::LM_WARNING;
        color = Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN;
        break;
    case ESwiftLogLevelMessage:
        level = Mengine::LM_MESSAGE;
        color = Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE;
        break;
    case ESwiftLogLevelInfo:
#if defined(MENGINE_DEBUG)
        level = Mengine::LM_INFO;
        color = Mengine::LCOLOR_BLUE;
        break;
#else
        return;
#endif
    default:
        MENGINE_ASSERTION_FATAL( false, "invalid Swift log level '%ld'"
            , static_cast<long>(_level)
        );

        return;
    }

#if !defined(MENGINE_DEBUG)
    _file = nil;
    _line = 0;
    _function = nil;
#endif

    NSString * category = _category();
    NSString * message = _message();

    AppleLogRecordParam * record = [[AppleLogRecordParam alloc] init];

    record.LOG_TIMESTAMP = [AppleDetail getTimestamp];
    record.LOG_CATEGORY = category;
    record.LOG_THREAD = [NSThread isMainThread] ? @"Main" : [[NSThread currentThread] name];
    record.LOG_LEVEL = level;
    record.LOG_FLAG = Mengine::LFLAG_SHORT;
    record.LOG_FILTER = Mengine::LFILTER_NONE;
    record.LOG_COLOR = color;
    record.LOG_FILE = _file;
    record.LOG_LINE = _line;
    record.LOG_FUNCTION = _function;
    record.LOG_DATA = message;

    [iOSDetail log:record];
}
