#pragma once

#include "Kernel/LoggerLevel.h"

#import <Foundation/Foundation.h>

@interface iOSLog : NSObject

+ (void)logWithCategory:(NSString * _Nonnull)category
                  level:(Mengine::ELoggerLevel)level
                   flag:(uint32_t)flag
                 filter:(uint32_t)filter
                  color:(uint32_t)color
               function:(NSString * _Nullable)function
                   line:(int32_t)line
                 format:(NSString * _Nonnull)format, ... NS_FORMAT_FUNCTION(8, 9);

#define IOS_LOGGER_MESSAGE( ... ) [iOSLog logWithCategory:@(MENGINE_CODE_LIBRARY) level:Mengine::LM_MESSAGE flag:Mengine::LFLAG_SHORT filter:Mengine::LFILTER_NONE color:(Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE) function:@(MENGINE_CODE_FUNCTION) line:(MENGINE_CODE_LINE) format:__VA_ARGS__]

@end
