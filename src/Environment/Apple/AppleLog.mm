#import "AppleLog.h"

#include "Kernel/LoggerMessage.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/StdArg.h"

#define MENGINE_APPLE_LOG_MAX_MESSAGE 1024

@implementation AppleLog

+ (void)withFormat:(NSString *)_format, ... {
    MENGINE_VA_LIST_TYPE args;
    MENGINE_VA_LIST_START( args, _format );
            
    [AppleLog withFormatV:_format arguments:args];
    
    MENGINE_VA_LIST_END( args );
}

+ (void)withFormatV:(NSString *)format arguments:(va_list)args
{
    NSLogv(format, args);
}

+ (void) withRecord:(AppleLogRecordParam * _Nonnull)record {
#if defined(MENGINE_MASTER_RELEASE)
    if( record.LOG_LEVEL >= Mengine::LM_INFO )
    {
        return;
    }
#endif
    
    NSMutableString * buffer = [[NSMutableString alloc] initWithCapacity:512];


#if defined(MENGINE_DEBUG)
    if( record.LOG_LEVEL == Mengine::LM_ERROR )
    {
        [buffer appendString:@"🔴 "];
    }
    else if( record.LOG_LEVEL == Mengine::LM_WARNING )
    {
        [buffer appendString:@"🟡 "];
    }
#endif
    
    if( record.LOG_FLAG & Mengine::LFLAG_TIMESTAMP )
    {
        NSDate * date = [NSDate dateWithTimeIntervalSince1970:record.LOG_TIMESTAMP / 1000];
        NSDateFormatter * formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:[NSString stringWithFormat:@"[HH:mm:ss:%04lld] ", record.LOG_TIMESTAMP % 1000]];
        NSString * datestamp = [formatter stringFromDate:date];
        
        [buffer appendString:datestamp];
    }

    if( record.LOG_FLAG & Mengine::LFLAG_SYMBOLSTAMP )
    {
        Mengine::ELoggerLevel level = record.LOG_LEVEL;

        Mengine::Char symbol = Mengine::Helper::getLoggerLevelSymbol( level );

        [buffer appendFormat:@"%c ", symbol];
    }

    if( record.LOG_FLAG & Mengine::LFLAG_CATEGORYSTAMP )
    {
        NSString * category = record.LOG_CATEGORY;
        
        [buffer appendFormat:@"[%@] ", category];
    }
    
    if( record.LOG_FLAG & Mengine::LFLAG_THREADSTAMP )
    {
        NSString * threadName = record.LOG_THREAD;
        
        [buffer appendFormat:@"|%@| ", threadName];
    }
    
    if( record.LOG_FLAG & Mengine::LFLAG_FILESTAMP )
    {
        NSString * file = record.LOG_FILE;
        int32_t line = record.LOG_LINE;
        
        if( file != nil )
        {
            [buffer appendFormat:@"%@[%d] ", file, line];
        }
    }

    if( record.LOG_FLAG & Mengine::LFLAG_FUNCTIONSTAMP )
    {
        NSString * function = record.LOG_FUNCTION;
        
        if( function != nil )
        {
            [buffer appendFormat:@"%@ ", function];
        }
    }

    NSString * data = record.LOG_DATA;
    
    [buffer appendString:data];
    
    size_t buffer_len = [buffer length];

    size_t buffer_packages = buffer_len / MENGINE_APPLE_LOG_MAX_MESSAGE;
    size_t buffer_tail = buffer_len % MENGINE_APPLE_LOG_MAX_MESSAGE;

    if( buffer_packages != 0 )
    {
        NSLog(@"%@ <<<", [buffer substringWithRange:NSMakeRange(0, MENGINE_APPLE_LOG_MAX_MESSAGE)]);

        for( size_t package = 1; package != buffer_packages; ++package )
        {
            NSLog(@">>>  %@", [buffer substringWithRange:NSMakeRange(package * MENGINE_APPLE_LOG_MAX_MESSAGE, MENGINE_APPLE_LOG_MAX_MESSAGE)]);
        }

        if( buffer_tail != 0 )
        {
            NSLog(@">>>  %@", [buffer substringFromIndex:buffer_packages * MENGINE_APPLE_LOG_MAX_MESSAGE]);
        }
    }
    else
    {
        NSLog(@"%@", [buffer substringFromIndex:buffer_packages * MENGINE_APPLE_LOG_MAX_MESSAGE]);
    }
}

@end
