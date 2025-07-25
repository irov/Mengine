#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/LoggerLevel.h"

#include "Config/Timestamp.h"

@interface AppleLogRecordParam : NSObject

@property (nonatomic) Mengine::Timestamp LOG_TIMESTAMP;
@property (nonatomic, strong) NSString * _Nonnull LOG_CATEGORY;
@property (nonatomic, strong) NSString * _Nonnull LOG_THREAD;
@property (nonatomic) Mengine::ELoggerLevel LOG_LEVEL;
@property (nonatomic) uint32_t LOG_FLAG;
@property (nonatomic) uint32_t LOG_FILTER;
@property (nonatomic) uint32_t LOG_COLOR;
@property (nonatomic, strong) NSString * _Nullable LOG_FILE;
@property (nonatomic) int32_t LOG_LINE;
@property (nonatomic, strong) NSString * _Nullable LOG_FUNCTION;
@property (nonatomic, strong) NSString * _Nonnull LOG_DATA;

@end
