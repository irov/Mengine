#import "iOSUserNotificationCenterPlugin.h"

#import "Environment/Apple/AppleBundle.h"
#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/Assertion.h"
#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSUserNotificationCenterScriptEmbedding.h"
#endif

#include "Config/StdMath.h"
#include "Config/StdLimits.h"

#import <UserNotifications/UserNotifications.h>

#define PLUGIN_BUNDLE_NAME "MengineiOSUserNotificationCenterPlugin"

static NSString * const MENGINE_LOCAL_NOTIFICATION_PREFIX = @"mengine.local.";
static NSString * const MENGINE_LOCAL_NOTIFICATION_LAUNCH_COUNT = @"mengine.usernotificationcenter.launch_count";
#if defined(MENGINE_DEBUG)
static NSTimeInterval const MENGINE_LOCAL_NOTIFICATION_DEBUG_INTERVAL = 5.0;
#endif

@implementation iOSUserNotificationCenterPlugin
{
    NSArray<NSDictionary *> * m_notifications;
    NSMutableDictionary<NSString *, NSDate *> * m_delayDates;
    NSUInteger m_scheduleGeneration;
}

+ (instancetype)sharedInstance {
    static iOSUserNotificationCenterPlugin *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSUserNotificationCenterPlugin class]];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    m_notifications = @[];
    m_delayDates = [NSMutableDictionary dictionary];
    m_scheduleGeneration = 0;

    return self;
}

- (void)requestAuthorization:(void (^ _Nonnull)(BOOL granted))completion {
    __weak iOSUserNotificationCenterPlugin * weakSelf = self;

    [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull operationCompletion)(void)) {
        iOSUserNotificationCenterPlugin * strongSelf = weakSelf;

        if (strongSelf == nil) {
            completion(NO);
            operationCompletion();
            return;
        }

        UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

        [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge)
                              completionHandler:^(BOOL granted, NSError * _Nullable error) {
            [AppleDetail addMainQueueOperation:^{
                if (error != nil) {
                    IOS_LOGGER_MESSAGE(@"Notification permission request failed: %@"
                       , [AppleDetail getMessageFromNSError:error]
                    );

                    strongSelf.m_notificationPermissionGranted = NO;
                    completion(NO);
                    operationCompletion();
                    return;
                }

                strongSelf.m_notificationPermissionGranted = granted;
                completion(granted);
                operationCompletion();
            }];
        }];
    }];
}

- (void)setNotificationDelay:(NSString *)identifier delay:(NSTimeInterval)delay {
    MENGINE_ASSERTION_FATAL( Mengine::StdMath::isfinite( delay ) == true, "invalid local notification '%s' delay %f"
        , [identifier UTF8String]
        , delay
    );

    @synchronized (m_delayDates) {
        if (delay <= 0.0) {
            [m_delayDates removeObjectForKey:identifier];

            return;
        }

        m_delayDates[identifier] = [NSDate dateWithTimeIntervalSinceNow:delay];
    }
}

#pragma mark - Local notifications

+ (NSString *)requestIdentifier_:(NSDictionary *)desc {
    NSNumber * identifier = desc[@"id"];
    NSString * requestIdentifier = [NSString stringWithFormat:@"%@%@", MENGINE_LOCAL_NOTIFICATION_PREFIX, identifier];

    return requestIdentifier;
}

+ (BOOL)isAuthorized_:(UNAuthorizationStatus)status {
    switch (status) {
        case UNAuthorizationStatusAuthorized:
        case UNAuthorizationStatusProvisional:
        case UNAuthorizationStatusEphemeral:
            return YES;
        default:
            return NO;
    }
}

+ (NSString *)notificationLanguage_ {
    NSArray<NSString *> * localizations = [NSLocale preferredLanguages];
    NSString * localization = [localizations firstObject];

    if (localization == nil) {
        return @"";
    }

    NSArray<NSString *> * components = [localization componentsSeparatedByString:@"-"];
    NSString * language = [components firstObject];
    NSString * normalizedLanguage = [language lowercaseString];

    return normalizedLanguage;
}

+ (NSString *)localizedString_:(NSString *)key {
    NSString * value = [[NSBundle mainBundle] localizedStringForKey:key value:nil table:nil];

    if ([value isEqualToString:key] == YES) {
        NSString * missingValue = [key stringByAppendingString:@".missing"];
        value = [[NSBundle mainBundle] localizedStringForKey:key value:missingValue table:nil];

        if ([value isEqualToString:missingValue] == YES) {
            value = nil;
        }
    }

    if (value == nil) {
        IOS_LOGGER_WARNING(@"local notification text not found: %@", key);

        return nil;
    }

    return value;
}

+ (BOOL)isValidNotification_:(NSDictionary *)desc {
    if ([desc isKindOfClass:[NSDictionary class]] == NO) {
        return NO;
    }

    NSArray<NSString *> * stringKeys = @[@"title", @"content"];

    for (NSString * key in stringKeys) {
        NSString * value = desc[key];

        if ([value isKindOfClass:[NSString class]] == NO) {
            return NO;
        }

        if (value.length == 0) {
            return NO;
        }
    }

    NSArray<NSString *> * numberKeys = @[@"id", @"delay", @"month", @"day", @"hour", @"minute", @"script"];

    for (NSString * key in numberKeys) {
        NSNumber * number = desc[key];

        if (number == nil) {
            continue;
        }

        if ([number isKindOfClass:[NSNumber class]] == NO) {
            return NO;
        }

        double value = [number doubleValue];

        if (Mengine::StdMath::isfinite( value ) == false) {
            return NO;
        }

        long long integerValue = [number longLongValue];

        if (value != integerValue) {
            return NO;
        }
    }

    long long identifier = [desc[@"id"] longLongValue];
    long long maximumIdentifier = Mengine::StdLimits::numeric_limits<int32_t>::max();

    if (identifier <= 0) {
        return NO;
    }

    if (identifier > maximumIdentifier) {
        return NO;
    }

    long long delay = [desc[@"delay"] longLongValue];
    long long maximumDelay = Mengine::StdLimits::numeric_limits<int64_t>::max() / 1000;

    if (delay < 0) {
        return NO;
    }

    if (delay > maximumDelay) {
        return NO;
    }

    NSString * languages = desc[@"languages"];

    if (languages != nil) {
        if ([languages isKindOfClass:NSString.class] == NO) {
            return NO;
        }
    }

    NSString * repeat = desc[@"repeat"];

    if (repeat != nil) {
        if ([repeat isKindOfClass:NSString.class] == NO) {
            return NO;
        }

        if ([repeat isEqualToString:@"once"] == NO) {
            if ([repeat isEqualToString:@"yearly"] == NO) {
                return NO;
            }
        }
    }

    BOOL yearly = [repeat isEqualToString:@"yearly"];

    if (yearly == YES) {
        if ([desc[@"month"] integerValue] <= 0) {
            return NO;
        }
    }

    long long script = [desc[@"script"] longLongValue];

    if (script < 0) {
        return NO;
    }

    if (script > 1) {
        return NO;
    }

    if (script == 1) {
        NSString * name = desc[@"name"];

        if ([name isKindOfClass:NSString.class] == NO) {
            return NO;
        }

        if (name.length == 0) {
            return NO;
        }
    }

    if ([desc[@"month"] integerValue] > 0) {
        NSInteger leapYear = 2000;
        NSDateComponents * components = [[NSDateComponents alloc] init];
        components.year = leapYear;
        components.month = [desc[@"month"] integerValue];
        components.day = [desc[@"day"] integerValue];
        components.hour = [desc[@"hour"] integerValue];
        components.minute = [desc[@"minute"] integerValue];

        NSCalendar * calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
        calendar.timeZone = [NSTimeZone timeZoneForSecondsFromGMT:0];

        if ([components isValidDateInCalendar:calendar] == NO) {
            return NO;
        }
    }

    NSUInteger triggers = 0;

    if (delay > 0) {
        ++triggers;
    }

    if ([desc[@"month"] integerValue] > 0) {
        ++triggers;
    }

    if (script == 1) {
        ++triggers;
    }

    if (triggers != 1) {
        return NO;
    }

    return YES;
}

- (NSArray<NSDictionary *> *)loadNotifications_ {
    NSURL * url = [[NSBundle mainBundle] URLForResource:@"local_notifications" withExtension:@"plist"];

    if (url == nil) {
        return @[];
    }

    NSError * error = nil;
    NSArray * notifications = [NSArray arrayWithContentsOfURL:url error:&error];

    if (notifications == nil) {
        IOS_LOGGER_WARNING(@"invalid local_notifications.plist: %@", [AppleDetail getMessageFromNSError:error]);

        return @[];
    }

    NSMutableArray<NSDictionary *> * valid = [NSMutableArray array];
    NSMutableSet<NSNumber *> * identifiers = [NSMutableSet set];

    for (NSDictionary * desc in notifications) {
        if ([iOSUserNotificationCenterPlugin isValidNotification_:desc] == NO) {
            IOS_LOGGER_WARNING(@"invalid local notification config (needs id, title, content and exactly one of delay, month/day or script): %@", desc);

            continue;
        }

        NSNumber * identifier = desc[@"id"];

        if ([identifiers containsObject:identifier] == YES) {
            IOS_LOGGER_WARNING(@"local notification duplicate id: %@", identifier);

            continue;
        }

        [valid addObject:desc];
        [identifiers addObject:identifier];
    }

    return valid;
}

- (NSArray<NSString *> *)notificationIdentifiers_ {
    NSMutableArray<NSString *> * identifiers = [NSMutableArray array];

    for (NSDictionary * desc in m_notifications) {
        NSString * identifier = [iOSUserNotificationCenterPlugin requestIdentifier_:desc];

        [identifiers addObject:identifier];
    }

    return identifiers;
}

- (void)removeNotifications_ {
    ++m_scheduleGeneration;

    NSArray<NSString *> * identifiers = [self notificationIdentifiers_];

    UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

    [center removePendingNotificationRequestsWithIdentifiers:identifiers];
    [center removeDeliveredNotificationsWithIdentifiers:identifiers];

    [center getPendingNotificationRequestsWithCompletionHandler:^(NSArray<UNNotificationRequest *> * requests) {
        NSMutableArray<NSString *> * staleIdentifiers = [NSMutableArray array];

        for (UNNotificationRequest * request in requests) {
            NSString * identifier = request.identifier;

            if ([identifier hasPrefix:MENGINE_LOCAL_NOTIFICATION_PREFIX] == NO) {
                continue;
            }

            if ([identifiers containsObject:identifier] == YES) {
                continue;
            }

            [staleIdentifiers addObject:identifier];
        }

        [center removePendingNotificationRequestsWithIdentifiers:staleIdentifiers];
    }];

    [center getDeliveredNotificationsWithCompletionHandler:^(NSArray<UNNotification *> * notifications) {
        NSMutableArray<NSString *> * staleIdentifiers = [NSMutableArray array];

        for (UNNotification * notification in notifications) {
            UNNotificationRequest * request = notification.request;
            NSString * identifier = request.identifier;

            if ([identifier hasPrefix:MENGINE_LOCAL_NOTIFICATION_PREFIX] == NO) {
                continue;
            }

            if ([identifiers containsObject:identifier] == YES) {
                continue;
            }

            [staleIdentifiers addObject:identifier];
        }

        [center removeDeliveredNotificationsWithIdentifiers:staleIdentifiers];
    }];
}

+ (UNNotificationRequest *)makeRequest_:(NSDictionary *)desc language:(NSString *)language delayDates:(NSDictionary<NSString *, NSDate *> *)delayDates now:(NSDate *)now {
    NSString * languages = desc[@"languages"];

    if (languages != nil) {
        NSArray<NSString *> * values = [languages componentsSeparatedByString:@","];
        NSCharacterSet * whitespace = [NSCharacterSet whitespaceAndNewlineCharacterSet];
        BOOL matches = NO;

        for (NSString * value in values) {
            NSString * trimmed = [value stringByTrimmingCharactersInSet:whitespace];

            if ([trimmed isEqualToString:language] == YES) {
                matches = YES;
                break;
            }
        }

        if (matches == NO) {
            return nil;
        }
    }

    UNNotificationTrigger * trigger = nil;

    if ([desc[@"script"] boolValue] == YES) {
        NSString * name = desc[@"name"];
        NSDate * date = delayDates[name];

        if (date == nil) {
            return nil;
        }

        NSTimeInterval interval = [date timeIntervalSinceDate:now];

        if (interval <= 0.0) {
            return nil;
        }

        trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:interval repeats:NO];
    } else if ([desc[@"month"] integerValue] > 0) {
        NSCalendar * calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];

        NSTimeZone * timeZone = [NSTimeZone localTimeZone];
        calendar.timeZone = timeZone;

        NSDateComponents * components = [[NSDateComponents alloc] init];
        components.calendar = calendar;
        components.month = [desc[@"month"] integerValue];
        components.day = [desc[@"day"] integerValue];
        components.hour = [desc[@"hour"] integerValue];
        components.minute = [desc[@"minute"] integerValue];

        NSInteger february = 2;
        NSInteger leapDay = 29;

        if (components.month == february) {
            if (components.day == leapDay) {
                NSInteger fifthWeekday = 5;
                components.weekdayOrdinal = fifthWeekday;
            }
        }

        BOOL yearly = [desc[@"repeat"] isEqualToString:@"yearly"];
        trigger = [UNCalendarNotificationTrigger triggerWithDateMatchingComponents:components repeats:yearly];
    } else {
        NSTimeInterval delay = [desc[@"delay"] doubleValue];

        trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:delay repeats:NO];
    }

    NSString * titleKey = desc[@"title"];
    NSString * bodyKey = desc[@"content"];
    NSString * title = [iOSUserNotificationCenterPlugin localizedString_:titleKey];
    NSString * body = [iOSUserNotificationCenterPlugin localizedString_:bodyKey];

    if (title == nil) {
        return nil;
    }

    if (body == nil) {
        return nil;
    }

    UNMutableNotificationContent * content = [[UNMutableNotificationContent alloc] init];
    content.title = title;
    content.body = body;
    content.sound = [UNNotificationSound defaultSound];

    NSString * requestIdentifier = [iOSUserNotificationCenterPlugin requestIdentifier_:desc];

    UNNotificationRequest * request = [UNNotificationRequest requestWithIdentifier:requestIdentifier content:content trigger:trigger];

    return request;
}

+ (NSArray<UNNotificationRequest *> *)makeRequests_:(NSArray<NSDictionary *> *)notifications language:(NSString *)language delayDates:(NSDictionary<NSString *, NSDate *> *)delayDates now:(NSDate *)now {
    NSMutableArray<UNNotificationRequest *> * requests = [NSMutableArray array];

#if defined(MENGINE_DEBUG)
    NSMutableDictionary<NSString *, NSDate *> * fireDates = [NSMutableDictionary dictionary];
#endif

    for (NSDictionary * desc in notifications) {
        UNNotificationRequest * request = [iOSUserNotificationCenterPlugin makeRequest_:desc language:language delayDates:delayDates now:now];

        if (request == nil) {
            continue;
        }

        NSDate * fireDate = nil;

        if ([desc[@"script"] boolValue] == YES) {
            NSString * name = desc[@"name"];
            fireDate = delayDates[name];
        } else if ([desc[@"month"] integerValue] > 0) {
            UNCalendarNotificationTrigger * trigger = (UNCalendarNotificationTrigger *)request.trigger;
            fireDate = trigger.nextTriggerDate;
        } else {
            NSTimeInterval delay = [desc[@"delay"] doubleValue];
            fireDate = [now dateByAddingTimeInterval:delay];
        }

        if (fireDate == nil) {
            continue;
        }

#if defined(MENGINE_DEBUG)
        fireDates[request.identifier] = fireDate;
#endif

        [requests addObject:request];
    }

#if defined(MENGINE_DEBUG)
    [requests sortUsingComparator:^NSComparisonResult(UNNotificationRequest * left, UNNotificationRequest * right) {
        NSDate * leftDate = fireDates[left.identifier];
        NSDate * rightDate = fireDates[right.identifier];
        NSComparisonResult order = [leftDate compare:rightDate];

        if (order != NSOrderedSame) {
            return order;
        }

        NSComparisonResult identifierOrder = [left.identifier compare:right.identifier options:NSNumericSearch];

        return identifierOrder;
    }];

    for (NSUInteger index = 0; index != requests.count; ++index) {
        UNNotificationRequest * request = requests[index];
        NSTimeInterval delay = (index + 1) * MENGINE_LOCAL_NOTIFICATION_DEBUG_INTERVAL;
        UNNotificationTrigger * trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:delay repeats:NO];
        UNNotificationRequest * debugRequest = [UNNotificationRequest requestWithIdentifier:request.identifier content:request.content trigger:trigger];
        requests[index] = debugRequest;
    }
#endif

    return requests;
}

- (void)scheduleNotifications_:(UIApplication *)application {
    if (m_notifications.count == 0) {
        return;
    }

    NSUInteger generation = ++m_scheduleGeneration;

    NSArray<NSDictionary *> * notifications = m_notifications;
    NSArray<NSString *> * identifiers = [self notificationIdentifiers_];
    NSString * language = [iOSUserNotificationCenterPlugin notificationLanguage_];

    NSDictionary<NSString *, NSDate *> * delayDates = nil;

    @synchronized (m_delayDates) {
        delayDates = [m_delayDates copy];
    }

    __weak iOSUserNotificationCenterPlugin * weakSelf = self;

    __block UIBackgroundTaskIdentifier backgroundTask = [application beginBackgroundTaskWithName:@"MengineLocalNotifications" expirationHandler:^{
        if (backgroundTask != UIBackgroundTaskInvalid) {
            [application endBackgroundTask:backgroundTask];
            backgroundTask = UIBackgroundTaskInvalid;
        }
    }];

    dispatch_block_t finishBackgroundTask = ^{
        if (backgroundTask == UIBackgroundTaskInvalid) {
            return;
        }

        [application endBackgroundTask:backgroundTask];
        backgroundTask = UIBackgroundTaskInvalid;
    };

    UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings * _Nonnull settings) {
        [AppleDetail addMainQueueOperation:^{
            iOSUserNotificationCenterPlugin * strongSelf = weakSelf;

            if (strongSelf == nil) {
                finishBackgroundTask();
                return;
            }

            if (strongSelf->m_scheduleGeneration != generation) {
                finishBackgroundTask();
                return;
            }

            UNAuthorizationStatus authorizationStatus = settings.authorizationStatus;
            BOOL authorized = [iOSUserNotificationCenterPlugin isAuthorized_:authorizationStatus];
            strongSelf.m_notificationPermissionGranted = authorized;

            if (authorized == NO) {
                finishBackgroundTask();
                return;
            }

            [center removePendingNotificationRequestsWithIdentifiers:identifiers];

            dispatch_group_t group = dispatch_group_create();
            NSDate * now = [NSDate date];
            NSArray<UNNotificationRequest *> * requests = [iOSUserNotificationCenterPlugin makeRequests_:notifications language:language delayDates:delayDates now:now];

            for (UNNotificationRequest * request in requests) {
                dispatch_group_enter(group);

                [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
                    if (error != nil) {
                        IOS_LOGGER_WARNING(@"local notification %@ schedule failed: %@"
                            , request.identifier
                            , [AppleDetail getMessageFromNSError:error]
                        );
                    } else {
                        IOS_LOGGER_MESSAGE(@"local notification %@ scheduled", request.identifier);
                    }

                    dispatch_group_leave(group);
                }];
            }

            dispatch_group_notify(group, dispatch_get_main_queue(), finishBackgroundTask);
        }];
    }];
}

- (void)requestAuthorizationOnLaunch_ {
    __weak iOSUserNotificationCenterPlugin * weakSelf = self;

    [iOSDetail addDidBecomeActiveOperationWithCompletion:^(void (^ _Nonnull operationCompletion)(void)) {
        NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        NSInteger launchCount = [defaults integerForKey:MENGINE_LOCAL_NOTIFICATION_LAUNCH_COUNT] + 1;
        [defaults setInteger:launchCount forKey:MENGINE_LOCAL_NOTIFICATION_LAUNCH_COUNT];

        operationCompletion();

        NSInteger requestLaunch = [AppleBundle getPluginConfigInteger:@PLUGIN_BUNDLE_NAME withKey:@"RequestAuthorizationLaunch" withDefault:0];

        if (requestLaunch <= 0) {
            return;
        }

        if (launchCount < requestLaunch) {
            return;
        }

        [[UNUserNotificationCenter currentNotificationCenter] getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings * _Nonnull settings) {
            if (settings.authorizationStatus != UNAuthorizationStatusNotDetermined) {
                return;
            }

            [AppleDetail addMainQueueOperation:^{
                iOSUserNotificationCenterPlugin * strongSelf = weakSelf;

                if (strongSelf == nil) {
                    return;
                }

                [strongSelf requestAuthorization:^(BOOL granted) {
                    IOS_LOGGER_MESSAGE(@"local notifications authorization on launch %ld: %@"
                        , (long)launchCount
                        , granted == YES ? @"granted" : @"denied"
                    );
                }];
            }];
        }];
    }];
}

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    UNUserNotificationCenter * center = [UNUserNotificationCenter currentNotificationCenter];

    center.delegate = self;

    m_notifications = [self loadNotifications_];

    [self requestAuthorizationOnLaunch_];

    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [self scheduleNotifications_:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [self removeNotifications_];
}

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSUserNotificationCenterScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSUserNotificationCenterScriptEmbedding>();
#endif
}

#pragma mark - UNUserNotificationCenterDelegate Protocol

- (void)dispatchRemoteNotification_:(UNNotification *)notification opened:(BOOL)opened {
    UNNotificationRequest * request = notification.request;
    UNNotificationTrigger * trigger = request.trigger;
    if ([trigger isKindOfClass:UNPushNotificationTrigger.class] == NO) {
        return;
    }

    UNNotificationContent * content = request.content;
    NSDictionary * userInfo = content.userInfo;

    NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
    NSArray * plugins = [delegate getPlugins];
    for (id<iOSPluginInterface> plugin in plugins) {
        if ([plugin respondsToSelector:@selector(handleRemoteNotification:opened:)] == NO) {
            continue;
        }

        [plugin handleRemoteNotification:userInfo opened:opened];
    }
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0), tvos(10.0)) {

    NSLog(@"userNotificationCenter willPresentNotification identifier: %@ title: %@ body: %@"
        , notification.request.identifier
        , notification.request.content.title
        , notification.request.content.body
    );

    [self dispatchRemoteNotification_:notification opened:NO];

    completionHandler(UNNotificationPresentationOptionBanner | UNNotificationPresentationOptionList | UNNotificationPresentationOptionSound | UNNotificationPresentationOptionBadge);
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center didReceiveNotificationResponse:(UNNotificationResponse *)response withCompletionHandler:(void(^)(void))completionHandler API_AVAILABLE(macos(10.14), ios(10.0), watchos(3.0)) API_UNAVAILABLE(tvos) {

    NSLog(@"userNotificationCenter didReceiveNotificationResponse identifier: %@ title: %@ body: %@"
        , response.notification.request.identifier
        , response.notification.request.content.title
        , response.notification.request.content.body
    );

    NSString * actionIdentifier = response.actionIdentifier;
    if ([actionIdentifier isEqualToString:UNNotificationDefaultActionIdentifier] == NO) {
        completionHandler();

        return;
    }

    UNNotification * notification = response.notification;
    [self dispatchRemoteNotification_:notification opened:YES];

    completionHandler();
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center openSettingsForNotification:(nullable UNNotification *)notification API_AVAILABLE(macos(10.14), ios(12.0)) API_UNAVAILABLE(watchos, tvos) {

    NSLog(@"userNotificationCenter openSettingsForNotification");
}

@end
