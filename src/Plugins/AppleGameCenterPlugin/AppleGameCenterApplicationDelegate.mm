#import "AppleGameCenterApplicationDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation AppleGameCenterApplicationDelegate


- (instancetype)init {
    self = [super init];
    
    self.m_gameCenterAuthenticate = NO;
    self.m_authenticateSuccess = NO;
    
    self.m_achievementsSynchronization = NO;
    self.m_achievementsComplete = [[NSMutableArray alloc] init];
    
    return self;
}

#pragma mark - AppleGameCenterInterface

+ (instancetype)sharedInstance {
    static AppleGameCenterApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleGameCenterApplicationDelegate class]];
    });
    return sharedInstance;
}

- (void)connect:(id<AppleGameCenterConnectCallbackInterface>)callback {
    [self login:^(NSError * _error) {
        if (_error != nil) {
            self.m_gameCenterAuthenticate = false;
            
            [self.m_achievementsComplete removeAllObjects];
            
            IOS_LOGGER_ERROR( @"login error: '%@'"
                , [AppleDetail getMessageFromNSError:_error]
            );
            
            [AppleDetail dispatchMainQueue:^{
                [callback onAppleGameCenterAuthenticate:NO];
            }];
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"connect successful" );
        
        if (self.m_gameCenterAuthenticate == false) {
            self.m_gameCenterAuthenticate = true;
            
            [AppleDetail dispatchMainQueue:^{
                [callback onAppleGameCenterAuthenticate:YES];
            }];
        }
        
        self.m_achievementsSynchronization = false;
        
        [self.m_achievementsComplete removeAllObjects];
        
        [AppleDetail dispatchMainQueue:^{
            [callback onAppleGameCenterSynchronizate:NO];
        }];
        
        [self loadCompletedAchievements:^(NSError * _error, NSArray * _completedAchievements) {
            if (_error != nil) {
                self.m_achievementsSynchronization = false;
                
                IOS_LOGGER_ERROR( @"load completed achievements error: '%@'"
                    , [AppleDetail getMessageFromNSError:_error]
                );
                
                [AppleDetail dispatchMainQueue:^{
                    [callback onAppleGameCenterSynchronizate:NO];
                }];
                
                return;
            }
            
            if (_completedAchievements != nil) {
                for (NSString * ach in _completedAchievements) {
                    IOS_LOGGER_MESSAGE( @"completed achievement: '%@'"
                        , ach
                    );
                    
                    [self.m_achievementsComplete addObject:ach];
                }
            }
            
            self.m_achievementsSynchronization = true;
            
            if (callback != nil) {
                [AppleDetail dispatchMainQueue:^{
                    [callback onAppleGameCenterSynchronizate:YES];
                }];
            }
        }] ;
    }];
}

- (BOOL)isConnect {
    return self.m_gameCenterAuthenticate;
}

- (BOOL)isSynchronized {
    return self.m_achievementsSynchronization;
}

- (BOOL)reportAchievement:(NSString *)identifier percent:(double)percent response:(void(^)(BOOL))handler {
    IOS_LOGGER_MESSAGE( @"report achievement: '%@' [%lf]"
        , identifier
        , percent
    );

    BOOL result = [self reportAchievementIdentifier:identifier percentComplete:percent withBanner:YES response:^(NSError * _error) {
        if (_error != nil) {
            IOS_LOGGER_ERROR( @"response achievement '%@' percent: %lf error: %@"
               , identifier
               , percent
               , [AppleDetail getMessageFromNSError:_error]
            );
            
            [AppleDetail dispatchMainQueue:^{
                handler( NO );
            }];
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"response achievement '%@' percent: %lf successful"
            , identifier
            , percent
        );
        
        if (percent >= 100.0) {
            [self.m_achievementsComplete addObject:identifier];
        }
        
        [AppleDetail dispatchMainQueue:^{
            handler( YES );
        }];
    }];
    
    if (result == NO) {
        IOS_LOGGER_ERROR( @"invalid report achievement '%@' percent: %lf"
           , identifier
           , percent
        );
        
        return NO;
    }
    
    return YES;
}

- (BOOL)checkAchievement:(NSString *)identifier {
    if ([self.m_achievementsComplete containsObject:identifier] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)resetAchievements {
    BOOL result = [self resetAchievements:^(NSError * _error) {
        if (_error != nil) {
            IOS_LOGGER_ERROR( @"reset achievemnts error: '%s'"
                , [[AppleDetail getMessageFromNSError:_error] UTF8String]
            );
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"reset achievement successful" );
    }] ;
    
    return YES;
}

- (BOOL)login:(void(^)(NSError *))handler {
    GKLocalPlayer * localPlayer = [GKLocalPlayer localPlayer];
    
    [localPlayer setAuthenticateHandler:^(UIViewController* v, NSError * error) {
        if (error != nil) {
            self.m_authenticateSuccess = NO;
            
            [AppleDetail dispatchMainQueue:^{
                handler( error );
            }];
            
            return;
        }
        
        self.m_authenticateSuccess = YES;
        
        [AppleDetail dispatchMainQueue:^{
            handler( nil );
        }];
    }];
    
    return YES;
}

- (BOOL)loadCompletedAchievements:(void(^)(NSError *, NSArray *))handler {
    if (self.m_authenticateSuccess == NO) {
        return NO;
    }
    
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray<GKAchievement *> * achievements, NSError * error) {
        if (error != nil) {
            [AppleDetail dispatchMainQueue:^{
                handler( error, nil );
            }];
            
            return;
        }
        
        if (achievements != nil) {
            [AppleDetail dispatchMainQueue:^{
                handler( nil, nil );
            }];
            
            return;
        }
        
        NSMutableArray * cmpAch = [[NSMutableArray alloc] init];
        
        for (GKAchievement * ach in achievements) {
            if ([ach isCompleted] == YES) {
                [cmpAch addObject:[ach identifier]];
            }
        }
        
        [AppleDetail dispatchMainQueue:^{
            handler( nil, cmpAch );
        }];
    }];
    
    return YES;
}

- (BOOL)resetAchievements:(void(^)(NSError * error))handler {
    IOS_LOGGER_MESSAGE( @"try reset achievemnts" );
    
    if (self.m_authenticateSuccess == NO) {
        IOS_LOGGER_ERROR( @"invalid reset achievements" );
        
        return NO;
    }
    
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError * error) {
        if (error != nil) {
            IOS_LOGGER_ERROR( @"reset achievemnts error: '%@'"
                , [AppleDetail getMessageFromNSError:error]
            );
            
            [AppleDetail dispatchMainQueue:^{
                handler( error );
            }];
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"reset achievement successful" );
        
        [AppleDetail dispatchMainQueue:^{
            handler( nil );
        }];
    }];
    
    return YES;
}

- (BOOL)reportScore:(NSString *)identifier score:(int64_t)score response:(void(^)(NSError *))handler {
    IOS_LOGGER_MESSAGE( @"report score: '%@' value: %lld"
        , identifier
        , score
    );
    
	if (self.m_authenticateSuccess == NO) {
        IOS_LOGGER_ERROR( @"invalid report score '%@' value: %lld"
           , identifier
           , score
        );
        
        return NO;
    }
    
	GKScore * scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:identifier];
	scoreReporter.value = score;
    scoreReporter.context = 0;
    
    NSArray * scores = @[scoreReporter];
    [GKScore reportScores:scores withCompletionHandler:^(NSError * error) {
        if (error != nil) {
            IOS_LOGGER_ERROR( @"response score '%@' value: %lld error: %@"
                , identifier
                , score
                , [AppleDetail getMessageFromNSError:error]
            );
            
            [AppleDetail dispatchMainQueue:^{
                handler(error);
            }];
            
            return;
        }
        
        IOS_LOGGER_MESSAGE( @"response score '%@' value: %lld successful"
            , identifier
            , score
        );
        
        [AppleDetail dispatchMainQueue:^{
            handler(nil);
        }];
	}];
    
    return YES;
}

- (BOOL)reportAchievementIdentifier:(NSString *)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^)(NSError *))handler {
    if (self.m_authenticateSuccess == NO) {
        return NO;
    }
    
    GKAchievement * achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
    [achievement setShowsCompletionBanner:banner];
    [achievement setPercentComplete: percent];
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError * error) {
        [AppleDetail dispatchMainQueue:^{
            handler(error);
        }];
    }];
    
    return YES;
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //ToDo
    
    return YES;
}

#pragma mark GKGameCenterControllerDelegate

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [gameCenterViewController dismissViewControllerAnimated:YES completion:^{
        IOS_LOGGER_MESSAGE( @"game center view controller dismissed" );
        
        //ToDo
    }];
}

@end
