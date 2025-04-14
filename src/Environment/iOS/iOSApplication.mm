#import "iOSApplication.h"

#import "Environment/Apple/AppleKeyChain.h"
#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"

@implementation iOSApplication

+ (instancetype)sharedInstance {
    static iOSApplication * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[iOSApplication alloc] init];
    });
    return sharedInstance;
}

+ (void)setPersistentStringForKey:(NSString * _Nonnull)key value:(NSString * _Nullable)value {
    [AppleKeyChain setStringForKey:key value:value];
    [AppleUserDefaults setStringForKey:key value:value];
}

+ (NSString *)getPersistentStringForKey:(NSString *)key defaultValue:(NSString *)d {
    NSString * keychain_value = [AppleKeyChain getStringForKey:key defaultValue:nil];
    
    if (keychain_value != nil) {
        return keychain_value;
    }
    
    NSString * userdefault_value = [AppleUserDefaults getStringForKey:key defaultValue:d];
    
    return userdefault_value;
}

+ (void)setPersistentIntegerForKey:(NSString *)key value:(NSInteger)value {
    [AppleKeyChain setIntegerForKey:key value:value];
    [AppleUserDefaults setIntegerForKey:key value:value];
}

+ (NSInteger)getPersistentIntegerForKey:(NSString *)key defaultValue:(NSInteger)d {
    NSInteger keychain_value = [AppleKeyChain getIntegerForKey:key defaultValue:-1];
    
    if (keychain_value != -1) {
        return keychain_value;
    }
    
    NSInteger userdefault_value = [AppleUserDefaults getIntegerForKey:key defaultValue:d];
    
    return userdefault_value;
}

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions {
    NSString * install_key = [iOSApplication getPersistentStringForKey:@"mengine.install_key" defaultValue:nil];
    NSInteger install_timestamp = [iOSApplication getPersistentIntegerForKey:@"mengine.install_timestamp" defaultValue:-1];
    NSString * install_version = [iOSApplication getPersistentStringForKey:@"mengine.install_version" defaultValue:nil];
    NSInteger install_rnd = [iOSApplication getPersistentIntegerForKey:@"mengine.install_rnd" defaultValue:-1];
    NSInteger session_index = [iOSApplication getPersistentIntegerForKey:@"mengine.session_index" defaultValue:0];
    NSString * session_id = [iOSApplication getPersistentStringForKey:@"mengine.session_id" defaultValue:nil];
    NSString * user_id = [iOSApplication getPersistentStringForKey:@"mengine.user_id" defaultValue:session_id];
    
    if (install_key == nil) {
        NSString * randomHex = [AppleDetail getRandomHexString:16];
        
#if defined(MENGINE_DEBUG)
        install_key = [@"MDIK" stringByAppendingString:randomHex];
#else
        install_key = [@"MRIK" stringByAppendingString:randomHex];
#endif
        
        install_timestamp = [AppleDetail getTimestamp];
        install_version = [iOSDetail getBuildVersion];
        install_rnd = [AppleDetail getSecureRandomInteger];
        
        if (install_rnd == 0) {
            install_rnd = 1;
        } else if (install_rnd < 0) {
            install_rnd = -install_rnd;
        }
        
        [AppleKeyChain setStringForKey:@"mengine.install_key" value:install_key];
        [AppleKeyChain setIntegerForKey:@"mengine.install_timestamp" value:install_timestamp];
        [AppleKeyChain setStringForKey:@"mengine.install_version" value:install_version];
        [AppleKeyChain setIntegerForKey:@"mengine.install_rnd" value:install_rnd];
    }
    
    if (user_id == nil) {
        user_id = install_key;
        
        [AppleKeyChain setStringForKey:@"mengine.user_id" value:user_id];
    }
    
    NSInteger next_session_index = session_index + 1;
    
    [AppleKeyChain setIntegerForKey:@"mengine.session_index" value:next_session_index];
    
    self.m_installKey = install_key;
    self.m_installTimestamp = install_timestamp;
    self.m_installVersion = install_version;
    self.m_installRND = install_rnd;
    self.m_sessionIndex = session_index;
    self.m_sessionId = user_id;
    self.m_sessionTimestamp = [AppleDetail getTimestamp];
    
    return YES;
}

- (void)setSessionId:(NSString * _Nonnull)userId {
    if ([self.m_userId isEqualToString:sessionId] == YES) {
        return;
    }
    
    self.m_userId = userId;
    
    [AppleKeyChain setStringForKey:@"mengine.user_id" value:self.m_userId];
    
    iOSSessionIdParam * param = [iOSSessionIdParam alloc];
    param.USER_ID = self.m_userId;
    
    [iOSDetail setSessionId:param];
}

- (void)removeSessionData {
    [AppleKeyChain clear];
    [AppleUserDefaults clear];
    
    [iOSDetail removeSessionData];
}

- (NSString * _Nonnull)getInstallKey {
    return self.m_installKey;
}

- (NSInteger)getInstallTimestamp {
    return self.m_installTimestamp;
}

- (NSString * _Nonnull)getInstallVersion {
    return self.m_installVersion;
}

- (NSInteger)getInstallRND {
    return self.m_installRND;
}

- (NSInteger)getSessionIndex {
    return self.m_sessionIndex;
}

- (NSString * _Nonnull)getSessionId {
    return self.m_sessionId;
}

- (NSInteger)getSessionTimestamp {
    return self.m_sessionTimestamp;
}

@end
