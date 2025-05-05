#import "AppleKeyChain.h"

#import "Environment/Apple/AppleLog.h"

@implementation AppleKeyChain

+ (BOOL)setDataForKey:(NSString *)key data:(NSDate *)data {
    NSString * tag = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    
    NSDictionary * query = @{
        (__bridge id)kSecClass:(__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag,
        (__bridge id)kSecAttrAccount:key
    };
    
    OSStatus status_matching = ::SecItemCopyMatching((__bridge CFDictionaryRef)query, NULL);
    
    if (status_matching == errSecSuccess) {
        NSDictionary * attributesToUpdate = @{(__bridge id)kSecValueData:data};
        
        OSStatus status_update = ::SecItemUpdate((__bridge CFDictionaryRef)query, (__bridge CFDictionaryRef)attributesToUpdate);
        
        if (status_update != errSecSuccess) {
            CFStringRef errorMessage = ::SecCopyErrorMessageString(status_update, NULL);
            
            [AppleLog withFormat:@"Keychain %@ [setDataForKey] update failed: %@ (status code: %d)", key, errorMessage, (int)status_update];
            
            if (errorMessage) {
                ::CFRelease(errorMessage);
            }
            
            return NO;
        }
        
        return NO;
    }
    
    if (status_matching != errSecItemNotFound) {
        CFStringRef errorMessage = ::SecCopyErrorMessageString(status_matching, NULL);
        
        [AppleLog withFormat:@"Keychain %@ [setDataForKey] matching failed: %@ (status code: %d)", key, errorMessage, (int)status_matching];
        
        if (errorMessage) {
            ::CFRelease(errorMessage);
        }
        
        return NO;
    }
    
    NSDictionary * newItem = @{
        (__bridge id)kSecClass:(__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag,
        (__bridge id)kSecAttrAccount:key,
        (__bridge id)kSecValueData:data
    };

    OSStatus status_add = ::SecItemAdd((__bridge CFDictionaryRef)newItem, NULL);
    
    if (status_add != errSecSuccess) {
        CFStringRef errorMessage = ::SecCopyErrorMessageString(status_add, NULL);
        
        [AppleLog withFormat:@"Keychain %@ [setDataForKey] add failed: %@ (status code: %d)", key, errorMessage, (int)status_add];
        
        if (errorMessage) {
            ::CFRelease(errorMessage);
        }
        
        return NO;
    }
    
    return YES;
}

+ (NSData *)getDataForKey:(NSString *)key {
    NSString * tag = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    
    NSDictionary * query = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag,
        (__bridge id)kSecAttrAccount:key,
        (__bridge id)kSecReturnData:@YES,
        (__bridge id)kSecMatchLimit:(__bridge id)kSecMatchLimitOne
    };
    
    CFTypeRef result = NULL;
    OSStatus status_matching = ::SecItemCopyMatching((__bridge CFDictionaryRef)query, &result);
    
    if (status_matching == errSecItemNotFound) {
        return nil;
    }
    
    if (status_matching != errSecSuccess) {
        CFStringRef errorMessage = ::SecCopyErrorMessageString(status_matching, NULL);
        
        [AppleLog withFormat:@"Keychain %@ [getDataForKey] matching failed: %@ (status code: %d)", key, errorMessage, (int)status_matching];
        
        if (errorMessage) {
            ::CFRelease(errorMessage);
        }
        
        return nil;
    }
     
    NSData * data = (__bridge_transfer NSData *)result;
    
    return data;
}

+ (BOOL)setStringForKey:(NSString *)key value:(NSString *)value {
    NSData * data = [value dataUsingEncoding:NSUTF8StringEncoding];
    
    BOOL successful = [AppleKeyChain setDataForKey:key data:data];
    
    return successful;
}

+ (NSString *)getStringForKey:(NSString *)key defaultValue:(NSString *)defaultValue {
    NSData * data = [AppleKeyChain getDataForKey:key];
    
    if (data == nil) {
        return defaultValue;
    }
    
    NSString * value = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    return value;
}

+ (BOOL)setBooleanForKey:(NSString *)key value:(BOOL)value {
    NSData * data = [NSData dataWithBytes:&value length:sizeof(BOOL)];
    
    BOOL successful = [AppleKeyChain setDataForKey:key data:data];
    
    return successful;
}

+ (BOOL)getBooleanForKey:(NSString *)key defaultValue:(BOOL)defaultValue {
    NSData * data = [AppleKeyChain getDataForKey:key];
    
    if (data == nil) {
        return defaultValue;
    }
    
    if (data.length != sizeof(BOOL)) {
        return defaultValue;
    }
    
    BOOL value;
    [data getBytes:&value length:sizeof(BOOL)];

    return value;
}

+ (BOOL)setIntegerForKey:(NSString *)key value:(NSInteger)value {
    NSData * data = [NSData dataWithBytes:&value length:sizeof(NSInteger)];
    
    BOOL successful = [AppleKeyChain setDataForKey:key data:data];
    
    return successful;
}

+ (NSInteger)getIntegerForKey:(NSString *)key defaultValue:(NSInteger)defaultValue {
    NSData * data = [AppleKeyChain getDataForKey:key];
    
    if (data == nil) {
        return defaultValue;
    }
    
    if (data.length != sizeof(NSInteger)) {
        return defaultValue;
    }
    
    NSInteger value;
    [data getBytes:&value length:sizeof(NSInteger)];

    return value;
}

+ (BOOL)setDoubleForKey:(NSString *)key value:(double)value {
    NSData * data = [NSData dataWithBytes:&value length:sizeof(double)];
    
    BOOL successful = [AppleKeyChain setDataForKey:key data:data];
    
    return successful;
}

+ (double)getDoubleForKey:(NSString *)key defaultValue:(double)defaultValue {
    NSData * data = [AppleKeyChain getDataForKey:key];
    
    if (data == nil) {
        return defaultValue;
    }
    
    if (data.length != sizeof(double)) {
        return defaultValue;
    }
    
    double value;
    [data getBytes:&value length:sizeof(double)];

    return value;
}

+ (BOOL)setNSTimeIntervalForKey:(NSString *)key value:(NSTimeInterval)value {
    NSData * data = [NSData dataWithBytes:&value length:sizeof(NSTimeInterval)];
    
    BOOL successful = [AppleKeyChain setDataForKey:key data:data];
    
    return successful;
}

+ (NSTimeInterval)getNSTimeIntervalForKey:(NSString *)key defaultValue:(NSTimeInterval)defaultValue {
    NSData * data = [AppleKeyChain getDataForKey:key];
    
    if (data == nil) {
        return defaultValue;
    }
    
    if (data.length != sizeof(NSTimeInterval)) {
        return defaultValue;
    }
    
    NSTimeInterval value;
    [data getBytes:&value length:sizeof(NSTimeInterval)];

    return value;
}

+ (BOOL)hasKey:(NSString *)key {
    NSString * tag = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    
    NSDictionary * query = @{
        (__bridge id)kSecClass:(__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag,
        (__bridge id)kSecAttrAccount:key,
        (__bridge id)kSecReturnData:@NO,
        (__bridge id)kSecMatchLimit:(__bridge id)kSecMatchLimitOne
    };
    
    OSStatus status_matching = ::SecItemCopyMatching((__bridge CFDictionaryRef)query, NULL);
    
    if (status_matching == errSecSuccess) {
        return YES;
    }
    
    if (status_matching == errSecItemNotFound) {
        return NO;
    }
    
    CFStringRef errorMessage = ::SecCopyErrorMessageString(status_matching, NULL);
    
    [AppleLog withFormat:@"Keychain %@ [hasKey] matching failed: %@ (status code: %d)", key, errorMessage, (int)status_matching];
    
    if (errorMessage) {
        ::CFRelease(errorMessage);
    }
    
    return NO;
}

+ (BOOL)removeKey:(NSString *)key {
    NSString * tag = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    
    NSDictionary * query = @{
        (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag,
        (__bridge id)kSecAttrAccount:key
    };
    
    OSStatus status = ::SecItemDelete((__bridge CFDictionaryRef)query);
    
    if (status == errSecSuccess) {
        return YES;
    }
    
    if (status == errSecItemNotFound) {
        return YES;
    }
    
    CFStringRef errorMessage = ::SecCopyErrorMessageString(status, NULL);
    [AppleLog withFormat:@"Keychain %@ [removeKey] delete failed: %@ (status code: %d)", key, errorMessage, (int)status];
    
    if (errorMessage) {
        ::CFRelease(errorMessage);
    }
    
    return NO;
}

+ (BOOL)clear {
    NSString * tag = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    
    NSDictionary * query = @{
        (__bridge id)kSecClass:(__bridge id)kSecClassGenericPassword,
        (__bridge id)kSecAttrService:tag
    };
    
    OSStatus status = ::SecItemDelete((__bridge CFDictionaryRef)query);
    
    if (status == errSecSuccess) {
        return YES;
    }
    
    if (status == errSecItemNotFound) {
        return YES;
    }
    
    CFStringRef errorMessage = ::SecCopyErrorMessageString(status, NULL);
    [AppleLog withFormat:@"Keychain [clear] delete failed: %@ (status code: %d)", errorMessage, (int)status];
        
    if (errorMessage) {
        ::CFRelease(errorMessage);
    }
        
    return NO;
}

@end
