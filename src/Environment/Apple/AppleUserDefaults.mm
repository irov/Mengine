#import "AppleUserDefaults.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

@implementation AppleUserDefaults

+ (NSString *)getStringForKey:(NSString *)key defaultValue:(NSString *)defaultValue {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return defaultValue;
    }
    
    if ([defaults objectForKey:key] == nil) {
        return defaultValue;
    }
    
    NSString *value = [defaults stringForKey:key];
    
    return value;
}

+ (BOOL)setStringForKey:(NSString *)key value:(NSString *)value {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return NO;
    }
    
    [defaults setObject:value forKey:key];
    [defaults synchronize];
    
    return YES;
}

+ (BOOL)getBooleanForKey:(NSString *)key defaultValue:(BOOL)defaultValue {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return defaultValue;
    }
    
    if ([defaults objectForKey:key] == nil) {
        return defaultValue;
    }
    
    BOOL value = [defaults boolForKey:key];
    
    return value;
}

+ (BOOL)setBooleanForKey:(NSString *)key value:(BOOL)value {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return NO;
    }
    
    [defaults setBool:value forKey:key];
    [defaults synchronize];
    
    return YES;
}

+ (NSInteger)getIntegerForKey:(NSString *)key defaultValue:(NSInteger)defaultValue {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return defaultValue;
    }
    
    if ([defaults objectForKey:key] == nil) {
        return defaultValue;
    }
    
    NSInteger value = [defaults integerForKey:key];
    
    return value;
}

+ (BOOL)setIntegerForKey:(NSString *)key value:(NSInteger)value {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return NO;
    }
    
    [defaults setInteger:value forKey:key];
    [defaults synchronize];
    
    return YES;
}

+ (BOOL)hasKey:(NSString *)key {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return NO;
    }
    
    if ([defaults objectForKey:key] == nil) {
        return NO;
    }
    
    return YES;
}

+ (BOOL)removeKey:(NSString *)key {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (defaults == nil) {
        return NO;
    }

    [defaults removeObjectForKey:key];
    [defaults synchronize];
    
    return YES;
}

@end
