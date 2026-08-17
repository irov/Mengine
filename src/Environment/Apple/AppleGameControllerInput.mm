#import "AppleGameControllerInput.h"

#include "Interface/InputServiceInterface.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

#import <GameController/GameController.h>

#ifndef __IPHONE_OS_VERSION_MIN_REQUIRED
#   define __IPHONE_OS_VERSION_MIN_REQUIRED 0
#endif

#ifndef __TV_OS_VERSION_MIN_REQUIRED
#   define __TV_OS_VERSION_MIN_REQUIRED 0
#endif

#define MENGINE_APPLE_CONTROLLER_HAS_BUTTON_OPTIONS (__IPHONE_OS_VERSION_MIN_REQUIRED >= 130000 || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500 || __TV_OS_VERSION_MIN_REQUIRED >= 130000)
#define MENGINE_APPLE_CONTROLLER_HAS_THUMBSTICKS (__IPHONE_OS_VERSION_MIN_REQUIRED >= 120100 || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101401 || __TV_OS_VERSION_MIN_REQUIRED >= 120100)
//////////////////////////////////////////////////////////////////////////

@interface AppleGameControllerInput ()

@property (nonatomic, strong) NSMapTable<GCController *, NSNumber *> * m_controllerIds;
@property (nonatomic, strong) id m_connectObserver;
@property (nonatomic, strong) id m_disconnectObserver;
@property (nonatomic, assign) Mengine::ControllerId m_nextControllerId;
@property (nonatomic, assign) BOOL m_started;

- (void)connectController:(GCController *)controller;
- (void)disconnectController:(GCController *)controller;
- (void)configureController:(GCController *)controller controllerId:(Mengine::ControllerId)controllerId;

@end

@implementation AppleGameControllerInput

- (instancetype)init
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    self.m_controllerIds = [NSMapTable strongToStrongObjectsMapTable];
    self.m_nextControllerId = 1;
    self.m_started = NO;

    return self;
}

- (void)dealloc
{
    [self stop];
}

- (void)start
{
    if( self.m_started == YES )
    {
        return;
    }

    self.m_started = YES;

    NSNotificationCenter * notificationCenter = [NSNotificationCenter defaultCenter];
    __weak AppleGameControllerInput * weakSelf = self;

    self.m_connectObserver = [notificationCenter addObserverForName:GCControllerDidConnectNotification
                                                             object:nil
                                                              queue:[NSOperationQueue mainQueue]
                                                         usingBlock:^(NSNotification * notification) {
        [weakSelf connectController:(GCController *)notification.object];
    }];

    self.m_disconnectObserver = [notificationCenter addObserverForName:GCControllerDidDisconnectNotification
                                                                object:nil
                                                                 queue:[NSOperationQueue mainQueue]
                                                            usingBlock:^(NSNotification * notification) {
        [weakSelf disconnectController:(GCController *)notification.object];
    }];

    for( GCController * controller in [GCController controllers] )
    {
        [self connectController:controller];
    }
}

- (void)stop
{
    if( self.m_started == NO )
    {
        return;
    }

    self.m_started = NO;

    NSNotificationCenter * notificationCenter = [NSNotificationCenter defaultCenter];

    if( self.m_connectObserver != nil )
    {
        [notificationCenter removeObserver:self.m_connectObserver];
        self.m_connectObserver = nil;
    }

    if( self.m_disconnectObserver != nil )
    {
        [notificationCenter removeObserver:self.m_disconnectObserver];
        self.m_disconnectObserver = nil;
    }

    for( GCController * controller in self.m_controllerIds )
    {
        NSNumber * number = [self.m_controllerIds objectForKey:controller];
        Mengine::ControllerId controllerId = (Mengine::ControllerId)number.unsignedIntValue;

        controller.extendedGamepad.valueChangedHandler = nil;

        if( SERVICE_IS_INITIALIZE( Mengine::InputServiceInterface ) == true )
        {
            Mengine::Helper::pushControllerConnectEvent( Mengine::Helper::getSystemTimestamp(), controllerId, false );
        }
    }

    [self.m_controllerIds removeAllObjects];
}

- (void)connectController:(GCController *)controller
{
    if( controller == nil || [self.m_controllerIds objectForKey:controller] != nil )
    {
        return;
    }

    Mengine::ControllerId controllerId = self.m_nextControllerId++;
    [self.m_controllerIds setObject:@(controllerId) forKey:controller];

    [self configureController:controller controllerId:controllerId];

    if( SERVICE_IS_INITIALIZE( Mengine::InputServiceInterface ) == true )
    {
        Mengine::Helper::pushControllerConnectEvent( Mengine::Helper::getSystemTimestamp(), controllerId, true );
    }
}

- (void)disconnectController:(GCController *)controller
{
    NSNumber * number = [self.m_controllerIds objectForKey:controller];

    if( number == nil )
    {
        return;
    }

    Mengine::ControllerId controllerId = (Mengine::ControllerId)number.unsignedIntValue;

    controller.extendedGamepad.valueChangedHandler = nil;
    [self.m_controllerIds removeObjectForKey:controller];

    if( SERVICE_IS_INITIALIZE( Mengine::InputServiceInterface ) == true )
    {
        Mengine::Helper::pushControllerConnectEvent( Mengine::Helper::getSystemTimestamp(), controllerId, false );
    }
}

- (void)configureController:(GCController *)controller controllerId:(Mengine::ControllerId)controllerId
{
    controller.handlerQueue = dispatch_get_main_queue();

    GCExtendedGamepad * gamepad = controller.extendedGamepad;

    if( gamepad == nil )
    {
        return;
    }

    gamepad.valueChangedHandler = ^(GCExtendedGamepad * changedGamepad, GCControllerElement * element) {
        if( SERVICE_IS_INITIALIZE( Mengine::InputServiceInterface ) == false )
        {
            return;
        }

        const Mengine::Timestamp timestamp = Mengine::Helper::getSystemTimestamp();

        if( element == changedGamepad.buttonA )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_A, changedGamepad.buttonA.value, changedGamepad.buttonA.isPressed == YES );
        }
        else if( element == changedGamepad.buttonB )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_B, changedGamepad.buttonB.value, changedGamepad.buttonB.isPressed == YES );
        }
        else if( element == changedGamepad.buttonX )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_X, changedGamepad.buttonX.value, changedGamepad.buttonX.isPressed == YES );
        }
        else if( element == changedGamepad.buttonY )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_Y, changedGamepad.buttonY.value, changedGamepad.buttonY.isPressed == YES );
        }
        else if( element == changedGamepad.leftShoulder )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_LEFT_SHOULDER, changedGamepad.leftShoulder.value, changedGamepad.leftShoulder.isPressed == YES );
        }
        else if( element == changedGamepad.rightShoulder )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_RIGHT_SHOULDER, changedGamepad.rightShoulder.value, changedGamepad.rightShoulder.isPressed == YES );
        }
        else if( element == changedGamepad.leftTrigger )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_LEFT_TRIGGER, changedGamepad.leftTrigger.value, changedGamepad.leftTrigger.isPressed == YES );
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_LEFT_TRIGGER, changedGamepad.leftTrigger.value );
        }
        else if( element == changedGamepad.rightTrigger )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_RIGHT_TRIGGER, changedGamepad.rightTrigger.value, changedGamepad.rightTrigger.isPressed == YES );
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_RIGHT_TRIGGER, changedGamepad.rightTrigger.value );
        }
        else if( element == changedGamepad.buttonMenu )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_MENU, changedGamepad.buttonMenu.value, changedGamepad.buttonMenu.isPressed == YES );
        }
#if MENGINE_APPLE_CONTROLLER_HAS_BUTTON_OPTIONS
        else if( element == changedGamepad.buttonOptions )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_OPTIONS, changedGamepad.buttonOptions.value, changedGamepad.buttonOptions.isPressed == YES );
        }
#endif
        else if( element == changedGamepad.dpad )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_DPAD_UP, changedGamepad.dpad.up.value, changedGamepad.dpad.up.isPressed == YES );
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_DPAD_DOWN, changedGamepad.dpad.down.value, changedGamepad.dpad.down.isPressed == YES );
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_DPAD_LEFT, changedGamepad.dpad.left.value, changedGamepad.dpad.left.isPressed == YES );
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_DPAD_RIGHT, changedGamepad.dpad.right.value, changedGamepad.dpad.right.isPressed == YES );
        }
        else if( element == changedGamepad.leftThumbstick )
        {
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_LEFT_X, changedGamepad.leftThumbstick.xAxis.value );
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_LEFT_Y, changedGamepad.leftThumbstick.yAxis.value );
        }
        else if( element == changedGamepad.rightThumbstick )
        {
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_RIGHT_X, changedGamepad.rightThumbstick.xAxis.value );
            Mengine::Helper::pushControllerAxisEvent( timestamp, controllerId, Mengine::CA_RIGHT_Y, changedGamepad.rightThumbstick.yAxis.value );
        }
#if MENGINE_APPLE_CONTROLLER_HAS_THUMBSTICKS
        else if( element == changedGamepad.leftThumbstickButton )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_LEFT_THUMB, changedGamepad.leftThumbstickButton.value, changedGamepad.leftThumbstickButton.isPressed == YES );
        }
        else if( element == changedGamepad.rightThumbstickButton )
        {
            Mengine::Helper::pushControllerButtonEvent( timestamp, controllerId, Mengine::CB_RIGHT_THUMB, changedGamepad.rightThumbstickButton.value, changedGamepad.rightThumbstickButton.isPressed == YES );
        }
#endif
    };
}

@end
