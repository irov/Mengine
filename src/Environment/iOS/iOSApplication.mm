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

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions {
    NSString * install_key = [AppleKeyChain getStringForKey:@"mengine.app.install_key" defaultValue:nil];
    NSInteger install_timestamp = [AppleKeyChain getIntegerForKey:@"mengine.app.install_timestamp" defaultValue:-1];
    NSString * install_version = [AppleKeyChain getStringForKey:@"mengine.app.install_version" defaultValue:nil];
    NSInteger install_rnd = [AppleKeyChain getIntegerForKey:@"mengine.app.install_rnd" defaultValue:-1];
    NSInteger session_index = [AppleKeyChain getIntegerForKey:@"mengine.app.session_index" defaultValue:0];
    //ToDo remove session_id
    NSString * session_id = [AppleKeyChain getStringForKey:@"mengine.session_id" defaultValue:nil];
    NSString * user_id = [AppleKeyChain getStringForKey:@"mengine.app.user_id" defaultValue:session_id];
    
    if (install_key == nil) {
        NSString * randomHex = [AppleDetail getRandomHexString:32];
        
#if defined(MENGINE_DEBUG)
        install_key = [@"MDIK" stringByAppendingString:randomHex];
#else
        install_key = [@"MRIK" stringByAppendingString:randomHex];
#endif
        
        install_timestamp = [AppleDetail getTimestamp];
        install_version = [iOSDetail getBuildVersion];
        install_rnd = [AppleDetail getSecureRandomInteger];
                
        [AppleKeyChain setStringForKey:@"mengine.app.install_key" value:install_key];
        [AppleKeyChain setIntegerForKey:@"mengine.app.install_timestamp" value:install_timestamp];
        [AppleKeyChain setStringForKey:@"mengine.app.install_version" value:install_version];
        [AppleKeyChain setIntegerForKey:@"mengine.app.install_rnd" value:install_rnd];
    }
    
    if (user_id == nil) {
        user_id = install_key;
        
        [AppleKeyChain setStringForKey:@"mengine.app.user_id" value:user_id];
    }
    
    NSInteger next_session_index = session_index + 1;
    
    [AppleKeyChain setIntegerForKey:@"mengine.app.session_index" value:next_session_index];
    
    self.m_installKey = install_key;
    self.m_installTimestamp = install_timestamp;
    self.m_installVersion = install_version;
    self.m_installRND = install_rnd;
    self.m_sessionIndex = session_index;
    self.m_sessionId = [AppleDetail getRandomHexString:32];
    self.m_sessionTimestamp = [AppleDetail getTimestamp];
    self.m_sessionRND = [AppleDetail getSecureRandomInteger];
    
    self.m_userId = user_id;
    
    return YES;
}

- (void)setUserId:(NSString * _Nonnull)userId {
    if ([self.m_userId isEqualToString:userId] == YES) {
        return;
    }
    
    self.m_userId = userId;
    
    [AppleKeyChain setStringForKey:@"mengine.app.user_id" value:self.m_userId];
    
    iOSUserParam * param = [iOSUserParam alloc];
    param.USER_ID = self.m_userId;
    
    [iOSDetail setUserId:param];
}

- (void)removeUserData {
    [AppleKeyChain clear];
    [AppleUserDefaults clear];
    
    [iOSDetail removeUserData];
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

- (NSInteger)getSessionRND {
    return self.m_sessionRND;
}

- (NSString * _Nonnull)getUserId {
    return self.m_userId;
}

@end
