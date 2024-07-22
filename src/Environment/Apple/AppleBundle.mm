#import "AppleBundle.h"

@implementation AppleBundle

+ (NSString *)getIdentifier {
    NSBundle * mainBundle = [NSBundle mainBundle];
    
    NSString * bundleIdentifier = [mainBundle bundleIdentifier];
    
    return bundleIdentifier;
}

+ (BOOL)hasPluginConfig:(NSString *) _plugin {
    NSBundle * mainBundle = [NSBundle mainBundle];
    
    if ([mainBundle objectForInfoDictionaryKey:_plugin] == nil) {
        return NO;
    }

    return YES;
}

+ (NSDictionary *)getPluginConfig:(NSString *) _plugin {
    NSBundle * mainBundle = [NSBundle mainBundle];
    
    NSDictionary * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

    if (PluginBundle == nil) {
        return nil;
    }

    return PluginBundle;
}

+ (NSString *)getPluginConfigString:(NSString *) _plugin withKey:(NSString *) _key withDefault:(NSString *) _default {
    NSDictionary * PluginBundle = [AppleBundle getPluginConfig:_plugin];

    if (PluginBundle == nil) {
        return _default;
    }

    NSString * PluginBundle_Value = [PluginBundle objectForKey:_key];

    if (PluginBundle_Value == nil) {
        return _default;
    }

    return PluginBundle_Value;
}
//////////////////////////////////////////////////////////////////////////
+ (NSNumber *)getPluginConfigNumber:(NSString *) _plugin withKey:(NSString *) _key withDefault:(NSNumber *) _default {
    NSDictionary * PluginBundle = [AppleBundle getPluginConfig:_plugin];

    if (PluginBundle == nil) {
        return _default;
    }

    NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

    if (PluginBundle_Number == nil) {
        return _default;
    }

    return PluginBundle_Number;
}
//////////////////////////////////////////////////////////////////////////
+ (NSInteger)getPluginConfigInteger:(NSString *) _plugin withKey:(NSString *)_key withDefault:(NSInteger)_default {
    NSDictionary * PluginBundle = [AppleBundle getPluginConfig:_plugin];

    if (PluginBundle == nil) {
        return _default;
    }

    NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

    if (PluginBundle_Number == nil) {
        return _default;
    }

    NSInteger PluginBundle_Value = [PluginBundle_Number integerValue];
    
    return PluginBundle_Value;
}
//////////////////////////////////////////////////////////////////////////
+ (double)getPluginConfigDouble:(NSString *) _plugin withKey:(NSString *) _key withDefault:(double) _default {
    NSDictionary * PluginBundle = [AppleBundle getPluginConfig:_plugin];

    if (PluginBundle == nil) {
        return _default;
    }

    NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

    if (PluginBundle_Number == nil) {
        return _default;
    }

    double PluginBundle_Value = [PluginBundle_Number doubleValue];
    
    return PluginBundle_Value;
}

+ (BOOL)getPluginConfigBoolean:(NSString *) _plugin withKey:(NSString *) _key withDefault:(BOOL) _default {
    NSDictionary * PluginBundle = [AppleBundle getPluginConfig:_plugin];

    if (PluginBundle == nil) {
        return _default;
    }

    NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

    if (PluginBundle_Number == nil) {
        return _default;
    }

    BOOL PluginBundle_Value = [PluginBundle_Number boolValue];
    
    return PluginBundle_Value;
}

@end
