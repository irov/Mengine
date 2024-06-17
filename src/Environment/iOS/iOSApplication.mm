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
    NSString * install_key = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_key", nil);
    NSInteger install_timestamp = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_timestamp", -1);
    NSString * install_version = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_version", nil);
    NSInteger install_rnd = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_rnd", -1);
    NSInteger session_index = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.session_index", 0);
    NSString * session_id = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.session_id", nil);
    
    if (install_key == nil) {
#ifdef MENGINE_DEBUG
        install_key = [@"MDIK" stringByAppendingString:Mengine::Helper::AppleGetRandomHexString(16)];
#else
        install_key = [@"MNIK" stringByAppendingString:Mengine::Helper::AppleGetRandomHexString(16)];
#endif
        install_timestamp = Mengine::Helper::AppleCurrentTimeMillis();
        install_version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        install_rnd = Mengine::Helper::AppleGetSecureRandomInteger();
        
        if (install_rnd == 0) {
            install_rnd = 1;
        } else if (install_rnd < 0) {
            install_rnd = -install_rnd;
        }
        
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.install_key", install_key);
        Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.install_timestamp", install_timestamp);
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.install_version", install_version);
        Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.install_rnd", install_rnd);
    }
    
    if (session_id == nil) {
        session_id = install_key;
        
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.session_id", session_id);
    }
    
    Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.session_index", session_index + 1);
    
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
    
    Mengine::Helper::AppleSetUserDefaultsString(@"mengine.session_id", self.m_sessionId);
    
    Mengine::Helper::iOSPluginApplicationDelegateEventNotify( MengineEvent.EVENT_SESSION_ID, self.m_sessionId, nil );
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
