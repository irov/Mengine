#import "AppleLog.h"

#include "Kernel/StringCopy.h"

#include "Config/StdString.h"

@implementation AppleLog

+ (void)withFormat:(NSString *) _format, ... {
    MENGINE_VA_LIST_TYPE args;
    MENGINE_VA_LIST_START( args, _format );
            
    [AppleLog withFormatV:_format arguments:args];
    
    MENGINE_VA_LIST_END( args );
}

+ (void)withFormatV:(NSString *)format arguments:(va_list)args
{
    NSLogv(format, args);
}

@end
