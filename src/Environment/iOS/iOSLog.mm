#import "iOSLog.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLog.h"

#include "Kernel/ThreadHelper.h"

#include "Config/StdArg.h"

#import "iOSDetail.h"

@implementation iOSLog

+ (void)logWithCategory:(NSString * _Nonnull)category
                  level:(Mengine::ELoggerLevel)level
                   flag:(uint32_t)flag
                 filter:(uint32_t)filter
                  color:(uint32_t)color
                   file:(NSString * _Nullable)file
                   line:(int32_t)line
               function:(NSString * _Nullable)function
                 format:(NSString * _Nonnull)format, ... NS_FORMAT_FUNCTION(9, 10) {
    MENGINE_VA_LIST_TYPE args;
    MENGINE_VA_LIST_START( args, format );
    NSString * message = [[NSString alloc] initWithFormat:format arguments:args];
    MENGINE_VA_LIST_END( args );
        
    AppleLogRecordParam * record = [AppleLogRecordParam alloc];
    
    record.LOG_TIMESTAMP = [AppleDetail getTimestamp];
    record.LOG_CATEGORY = category;
    record.LOG_THREADNAME = [NSThread isMainThread] ? @"Main" : [[NSThread currentThread] name];
    record.LOG_LEVEL = level;
    record.LOG_FLAG = flag;
    record.LOG_FILTER = filter;
    record.LOG_COLOR = color;
    record.LOG_FILE = file;
    record.LOG_LINE = line;
    record.LOG_FUNCTION = function;
    record.LOG_DATA = message;
    
    [iOSDetail log:record];
}

@end
