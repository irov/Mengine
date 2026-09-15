#import "iOSVibratorPlugin.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import <CoreHaptics/CoreHaptics.h>

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "iOSVibratorScriptEmbedding.h"
#   include "Kernel/ScriptEmbeddingHelper.h"
#endif

@interface iOSVibratorPlugin ()

@property (atomic, assign) BOOL m_mute;

@end

@implementation iOSVibratorPlugin
{
    CHHapticEngine * m_engine;
    BOOL m_started;
    BOOL m_running;
}

+ (instancetype)sharedInstance {
    iOSVibratorPlugin * plugin = [iOSDetail getPluginDelegateOfClass:[iOSVibratorPlugin class]];

    return plugin;
}

- (void)engineStopped:(CHHapticEngineStoppedReason)reason {
    IOS_LOGGER_INFO(@"iOSVibrator", @"haptic engine stopped reason=%ld", (long)reason);

    m_started = NO;
}

- (void)engineReset {
    IOS_LOGGER_INFO(@"iOSVibrator", @"haptic engine reset");

    m_started = NO;

    [self getEngine];
}

- (CHHapticEngine *)getEngine {
    if (m_running == NO) {
        return nil;
    }

    if (self.m_mute == YES) {
        return nil;
    }

    if ([CHHapticEngine capabilitiesForHardware].supportsHaptics == NO) {
        IOS_LOGGER_INFO(@"iOSVibrator", @"hardware without haptics support");

        return nil;
    }

    UIApplication * application = [UIApplication sharedApplication];
    if (application.applicationState != UIApplicationStateActive) {
        IOS_LOGGER_WARNING(@"skip haptic [application state %ld]", (long)application.applicationState);

        return nil;
    }

    if (m_engine == nil) {
        NSError * error = nil;
        CHHapticEngine * engine = [[CHHapticEngine alloc] initAndReturnError:&error];

        if (engine == nil) {
            IOS_LOGGER_ERROR(@"invalid create haptic engine error: %@", error);

            return nil;
        }

        // The engine never mixes into the game's audio session.
        engine.playsHapticsOnly = YES;

        __weak iOSVibratorPlugin * weakSelf = self;

        engine.stoppedHandler = ^(CHHapticEngineStoppedReason reason) {
            [AppleDetail addMainQueueOperation:^{
                [weakSelf engineStopped:reason];
            }];
        };

        // The haptic server resets on media services failures: restart it for the next impact.
        engine.resetHandler = ^{
            [AppleDetail addMainQueueOperation:^{
                [weakSelf engineReset];
            }];
        };

        m_engine = engine;
    }

    if (m_started == NO) {
        NSError * error = nil;

        if ([m_engine startAndReturnError:&error] == NO) {
            IOS_LOGGER_ERROR(@"invalid start haptic engine error: %@", error);

            return nil;
        }

        m_started = YES;

        IOS_LOGGER_INFO(@"iOSVibrator", @"haptic engine started");
    }

    return m_engine;
}

- (void)stopEngine {
    if (m_started == NO) {
        return;
    }

    m_started = NO;

    [m_engine stopWithCompletionHandler:nil];
}

#pragma mark - iOSVibratorInterface

- (void)prepare {
    // Core Haptics prepares the hardware by running the engine, the same one for every impact.
    [AppleDetail addMainQueueOperation:^{
        [self getEngine];
    }];
}

- (void)impact:(CGFloat)intensity sharpness:(CGFloat)sharpness {
    if ((intensity >= 0.0 && intensity <= 1.0) == false) {
        IOS_LOGGER_ERROR(@"invalid impact intensity %f", (float)intensity);

        return;
    }

    if ((sharpness >= 0.0 && sharpness <= 1.0) == false) {
        IOS_LOGGER_ERROR(@"invalid impact sharpness %f", (float)sharpness);

        return;
    }

    [AppleDetail addMainQueueOperation:^{
        CHHapticEngine * engine = [self getEngine];

        if (engine == nil) {
            return;
        }

        CHHapticEventParameter * intensityParameter = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticIntensity
                                                                                                    value:(float)intensity];
        CHHapticEventParameter * sharpnessParameter = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticSharpness
                                                                                                    value:(float)sharpness];

        CHHapticEvent * event = [[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticTransient
                                                             parameters:@[intensityParameter, sharpnessParameter]
                                                           relativeTime:0.0];

        NSError * error = nil;
        CHHapticPattern * pattern = [[CHHapticPattern alloc] initWithEvents:@[event] parameters:@[] error:&error];

        if (pattern == nil) {
            IOS_LOGGER_ERROR(@"invalid create haptic pattern error: %@", error);

            return;
        }

        id<CHHapticPatternPlayer> player = [engine createPlayerWithPattern:pattern error:&error];

        if (player == nil) {
            IOS_LOGGER_ERROR(@"invalid create haptic player error: %@", error);

            return;
        }

        if ([player startAtTime:CHHapticTimeImmediate error:&error] == NO) {
            IOS_LOGGER_ERROR(@"invalid start haptic player error: %@", error);
        }
    }];
}

- (void)mute:(BOOL)mute {
    self.m_mute = mute;

    if (mute == YES) {
        [AppleDetail addMainQueueOperation:^{
            [self stopEngine];
        }];
    }
}

- (BOOL)isMute {
    return self.m_mute;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    return YES;
}

- (void)onRunBegin {
    m_running = YES;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSVibratorScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopBegin {
    m_running = NO;

    [self stopEngine];
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSVibratorScriptEmbedding>();
#endif
}

- (void)onFinalize {
    m_running = NO;

    [self stopEngine];

    m_engine = nil;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [self stopEngine];
}

@end
