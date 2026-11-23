#pragma once

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, ESwiftLogLevel) {
    ESwiftLogLevelError,
    ESwiftLogLevelWarning,
    ESwiftLogLevelMessage,
    ESwiftLogLevelInfo
};

FOUNDATION_EXPORT void MengineSwiftLogWrite( ESwiftLogLevel _level, NSString * _Nonnull (NS_NOESCAPE ^_Nonnull _category)(void), NSString * _Nonnull _file, int32_t _line, NSString * _Nonnull _function, NSString * _Nonnull (NS_NOESCAPE ^_Nonnull _message)(void) );
