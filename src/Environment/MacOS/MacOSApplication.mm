#import "MacOSApplication.h"

#include "Environment/Apple/AppleUserDefaults.h"
#include "Environment/Apple/AppleDetail.h"

@implementation MacOSApplication

+ (instancetype)sharedInstance {
    static MacOSApplication *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

- (void)initialize {
    NSString * install_key = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_key", nil);
    NSInteger install_timestamp = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_timestamp", -1);
    NSString * install_version = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.install_version", nil);
    NSInteger install_rnd = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.install_rnd", -1);
    NSInteger session_index = Mengine::Helper::AppleGetUserDefaultsInteger(@"mengine.session_index", 0);
    NSString * old_user_id = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.session_id", nil);
    NSString * user_id = Mengine::Helper::AppleGetUserDefaultsString(@"mengine.user_id", old_user_id);
    
    if (install_key == nil) {
        install_key = [@"MNIK" stringByAppendingString:Mengine::Helper::AppleGetRandomHexString(16)];
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
    
    if (user_id == nil) {
        user_id = install_key;
        
        Mengine::Helper::AppleSetUserDefaultsString(@"mengine.user_id", user_id);
    }
    
    Mengine::Helper::AppleSetUserDefaultsInteger(@"mengine.session_index", session_index + 1);
    
    self.m_installKey = install_key;
    self.m_installTimestamp = install_timestamp;
    self.m_installVersion = install_version;
    self.m_installRND = install_rnd;
    self.m_sessionIndex = session_index;
    self.m_userId = user_id;
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
