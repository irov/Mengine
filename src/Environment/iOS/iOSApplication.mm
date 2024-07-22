#import "iOSApplication.h"

#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"

@implementation iOSApplication

+ (instancetype)sharedInstance {
    static iOSApplication *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

- (BOOL)didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions {
    NSString * install_key = [AppleUserDefaults getStringForKey:@"mengine.install_key" defaultValue:nil];
    NSInteger install_timestamp = [AppleUserDefaults getIntegerForKey:@"mengine.install_timestamp" defaultValue:-1];
    NSString * install_version = [AppleUserDefaults getStringForKey:@"mengine.install_version" defaultValue:nil];
    NSInteger install_rnd = [AppleUserDefaults getIntegerForKey:@"mengine.install_rnd" defaultValue:-1];
    NSInteger session_index = [AppleUserDefaults getIntegerForKey:@"mengine.session_index" defaultValue:0];
    NSString * session_id = [AppleUserDefaults getStringForKey:@"mengine.session_id" defaultValue:nil];
    
    if (install_key == nil) {
        NSString * randomHex = [AppleDetail getRandomHexString:16];
        
#ifdef MENGINE_DEBUG
        install_key = [@"MDIK" stringByAppendingString:randomHex];
#else
        install_key = [@"MNIK" stringByAppendingString:randomHex];
#endif
        
        install_timestamp = [AppleDetail getCurrentTimeMillis];
        install_version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        install_rnd = [AppleDetail getSecureRandomInteger];
        
        if (install_rnd == 0) {
            install_rnd = 1;
        } else if (install_rnd < 0) {
            install_rnd = -install_rnd;
        }
        
        [AppleUserDefaults setStringForKey:@"mengine.install_key" value:install_key];
        [AppleUserDefaults setIntegerForKey:@"mengine.install_timestamp" value:install_timestamp];
        [AppleUserDefaults setStringForKey:@"mengine.install_version" value:install_version];
        [AppleUserDefaults setIntegerForKey:@"mengine.install_rnd" value:install_rnd];
    }
    
    if (session_id == nil) {
        session_id = install_key;
        
        [AppleUserDefaults setStringForKey:@"mengine.session_id" value:session_id];
    }
    
    [AppleUserDefaults setIntegerForKey:@"mengine.session_index" value:session_index + 1];
    
    self.m_installKey = install_key;
    self.m_installTimestamp = install_timestamp;
    self.m_installVersion = install_version;
    self.m_installRND = install_rnd;
    self.m_sessionIndex = session_index;
    self.m_sessionId = session_id;
    
    return YES;
}

- (void)setSessionId:(NSString * _Nonnull)sessionId {
    if ([self.m_sessionId isEqualToString:sessionId] == YES) {
        return;
    }
    
    self.m_sessionId = sessionId;
    
    [AppleUserDefaults setStringForKey:@"mengine.session_id" value:self.m_sessionId];
    
    [iOSDetail eventNotify:AppleEvent.EVENT_SESSION_ID args:@[self.m_sessionId]];
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

@end
