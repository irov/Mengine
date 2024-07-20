#pragma once

#include "Kernel/LoggerLevel.h"

#include "Config/Timestamp.h"

#import <Foundation/Foundation.h>

@interface iOSLogRecordParam : NSObject

@property (nonatomic) Mengine::Timestamp LOG_TIMESTAMP;
@property (nonatomic, strong) NSString * _Nonnull LOG_CATEGORY;
@property (nonatomic, strong) NSString * _Nonnull LOG_THREADNAME;
@property (nonatomic) Mengine::ELoggerLevel LOG_LEVEL;
@property (nonatomic) uint32_t LOG_FLAG;
@property (nonatomic) uint32_t LOG_FILTER;
@property (nonatomic) uint32_t LOG_COLOR;
@property (nonatomic, strong) NSString * _Nonnull LOG_FUNCTION;
@property (nonatomic) int32_t LOG_LINE;
@property (nonatomic, strong) NSString * _Nonnull LOG_DATA;

@end
