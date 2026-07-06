#import "MacOSApplication.h"

#import "Environment/Apple/AppleUserDefaults.h"
#import "Environment/Apple/AppleDetail.h"

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
    NSString * old_install_id = [AppleUserDefaults getStringForKey:@"mengine.install_id" defaultValue:nil]; //deprecated
    NSString * install_id = [AppleUserDefaults getStringForKey:@"mengine.install_id" defaultValue:old_install_id];
    NSInteger install_timestamp = [AppleUserDefaults getIntegerForKey:@"mengine.install_timestamp" defaultValue:-1];
    NSString * install_version = [AppleUserDefaults getStringForKey:@"mengine.install_version" defaultValue:nil];
    NSInteger install_rnd = [AppleUserDefaults getIntegerForKey:@"mengine.install_rnd" defaultValue:-1];
    NSInteger session_index = [AppleUserDefaults getIntegerForKey:@"mengine.session_index" defaultValue:0];
    NSString * old_user_id = [AppleUserDefaults getStringForKey:@"mengine.session_id" defaultValue:nil]; //deprecated
    NSString * user_id = [AppleUserDefaults getStringForKey:@"mengine.user_id" defaultValue:old_user_id];

    if (install_id == nil) {
#if defined(MENGINE_DEBUG)
        install_id = [@"MDID" stringByAppendingString:[AppleDetail getRandomHexString:32]];
#else
        install_id = [@"MRID" stringByAppendingString:[AppleDetail getRandomHexString:32]];
#endif
        install_timestamp = [AppleDetail getTimestamp];
        install_version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
        install_rnd = [AppleDetail getSecureRandomInteger];

        if (install_rnd == 0) {
            install_rnd = 1;
        } else if (install_rnd < 0) {
            install_rnd = -install_rnd;
        }

        [AppleUserDefaults setStringForKey:@"mengine.install_id" value:install_id];
        [AppleUserDefaults setIntegerForKey:@"mengine.install_timestamp" value:install_timestamp];
        [AppleUserDefaults setStringForKey:@"mengine.install_version" value:install_version];
        [AppleUserDefaults setIntegerForKey:@"mengine.install_rnd" value:install_rnd];
    }

    if (user_id == nil) {
        user_id = install_id;

        [AppleUserDefaults setStringForKey:@"mengine.user_id" value:user_id];
    }

    [AppleUserDefaults setIntegerForKey:@"mengine.session_index" value:session_index + 1];

    self.m_installId = install_id;
    self.m_installTimestamp = install_timestamp;
    self.m_installVersion = install_version;
    self.m_installRND = install_rnd;
    self.m_sessionIndex = session_index;
    self.m_sessionId = [AppleDetail getRandomHexString:32];
    self.m_sessionTimestamp = [AppleDetail getTimestamp];
    self.m_sessionRND = [AppleDetail getSecureRandomInteger];
}

- (NSString * _Nonnull)getInstallId {
    return self.m_installId;
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

@end
