#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/LoggerLevel.h"

@interface iOSLog : NSObject

+ (void)logWithCategory:(NSString * _Nonnull)category
                  level:(Mengine::ELoggerLevel)level
                   flag:(uint32_t)flag
                 filter:(uint32_t)filter
                  color:(uint32_t)color
                   file:(NSString * _Nullable)file
                   line:(int32_t)line
               function:(NSString * _Nullable)function
                 format:(NSString * _Nonnull)format, ... NS_FORMAT_FUNCTION(9, 10);

#if defined(MENGINE_DEBUG)
#   define IOS_LOGGER_ERROR( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_ERROR flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED) file:@(MENGINE_CODE_FILE) line:(MENGINE_CODE_LINE) function:@(MENGINE_CODE_FUNCTION) format:__VA_ARGS__]
#   define IOS_LOGGER_WARNING( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_WARNING flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN) file:@(MENGINE_CODE_FILE) line:(MENGINE_CODE_LINE) function:@(MENGINE_CODE_FUNCTION) format:__VA_ARGS__]
#   define IOS_LOGGER_MESSAGE( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_MESSAGE flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE) file:@(MENGINE_CODE_FILE) line:(MENGINE_CODE_LINE) function:@(MENGINE_CODE_FUNCTION) format:__VA_ARGS__]
#   define IOS_LOGGER_INFO( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_INFO flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_BLUE) file:@(MENGINE_CODE_FILE) line:(MENGINE_CODE_LINE) function:@(MENGINE_CODE_FUNCTION) format:__VA_ARGS__]
#else
#   define IOS_LOGGER_ERROR( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_ERROR flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED) file:nil line:0 function:nil format:__VA_ARGS__]
#   define IOS_LOGGER_WARNING( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_WARNING flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN) file:nil line:0 function:nil format:__VA_ARGS__]
#   define IOS_LOGGER_MESSAGE( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_MESSAGE flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE) file:nil line:0 function:nil format:__VA_ARGS__]
#   define IOS_LOGGER_INFO( ... )
#endif

@end
