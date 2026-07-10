#include "ImGUIMacOSPlatform.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/TimestampHelper.h"

#include "imgui.h"

#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import <GameController/GameController.h>
#import <dispatch/dispatch.h>

#include <cstring>

#ifndef __IPHONE_OS_VERSION_MIN_REQUIRED
#   define __IPHONE_OS_VERSION_MIN_REQUIRED 0
#endif

#ifndef __TV_OS_VERSION_MIN_REQUIRED
#   define __TV_OS_VERSION_MIN_REQUIRED 0
#endif

#define MENGINE_IMGUI_APPLE_HAS_BUTTON_OPTIONS (__IPHONE_OS_VERSION_MIN_REQUIRED >= 130000 || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500 || __TV_OS_VERSION_MIN_REQUIRED >= 130000)
#define MENGINE_IMGUI_APPLE_HAS_CONTROLLER (__IPHONE_OS_VERSION_MIN_REQUIRED >= 140000 || __MAC_OS_X_VERSION_MIN_REQUIRED >= 110000 || __TV_OS_VERSION_MIN_REQUIRED >= 140000)
#define MENGINE_IMGUI_APPLE_HAS_THUMBSTICKS (__IPHONE_OS_VERSION_MIN_REQUIRED >= 120100 || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101401 || __TV_OS_VERSION_MIN_REQUIRED >= 120100)

@interface MengineImGUIMacOSPlatform()
{
    double m_time;
    NSCursor * m_mouseCursors[ImGuiMouseCursor_COUNT];
    BOOL m_mouseCursorHidden;
    NSWindow * m_window;
    ImVector<char> m_clipboard;
}

+ (ImGuiKey)keyCodeToImGuiKey:(int)_keyCode;
+ (ImGuiMouseSource)mouseSourceForEvent:(NSEvent *)_event;
+ (void)updateGamepads;
+ (void)addInputCharactersForEvent:(NSEvent *)_event;

- (BOOL)initializeWithView:(NSView *)_view;
- (void)finalizeBackend;
- (BOOL)isInitialized;
- (void)newFrameWithView:(NSView *)_view;
- (BOOL)handleEvent:(NSEvent *)_event view:(NSView *)_view;
- (const char *)clipboardText;
- (void)updateMouseCursor;
- (void)onApplicationBecomeActive:(NSNotification *)_notification;
- (void)onApplicationBecomeInactive:(NSNotification *)_notification;

@end

@interface NSCursor()
+ (id)_windowResizeNorthWestSouthEastCursor;
+ (id)_windowResizeNorthEastSouthWestCursor;
+ (id)_windowResizeNorthSouthCursor;
+ (id)_windowResizeEastWestCursor;
+ (id)busyButClickableCursor;
@end

//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIMacOSPlatform

+ (instancetype)sharedInstance
{
    static MengineImGUIMacOSPlatform * instance = nil;
    static dispatch_once_t onceToken;

    dispatch_once( &onceToken, ^{
        instance = [[MengineImGUIMacOSPlatform alloc] init];
    } );

    return instance;
}
//////////////////////////////////////////////////////////////////////////
- (instancetype)init
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    m_time = 0.0;
    m_mouseCursorHidden = NO;
    m_window = nil;
    std::memset( m_mouseCursors, 0, sizeof( m_mouseCursors ) );

    return self;
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)initializeWithView:(NSView *)_view
{
    return [[MengineImGUIMacOSPlatform sharedInstance] initializeWithView:_view];
}
//////////////////////////////////////////////////////////////////////////
+ (void)finalizeBackend
{
    [[MengineImGUIMacOSPlatform sharedInstance] finalizeBackend];
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)isInitialized
{
    return [[MengineImGUIMacOSPlatform sharedInstance] isInitialized];
}
//////////////////////////////////////////////////////////////////////////
+ (void)newFrameWithView:(NSView *)_view
{
    [[MengineImGUIMacOSPlatform sharedInstance] newFrameWithView:_view];
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)handleEvent:(NSEvent *)_event view:(NSView *)_view
{
    return [[MengineImGUIMacOSPlatform sharedInstance] handleEvent:_event view:_view];
}
//////////////////////////////////////////////////////////////////////////
+ (ImGuiKey)keyCodeToImGuiKey:(int)_keyCode
{
    switch( _keyCode )
    {
    case kVK_ANSI_A: return ImGuiKey_A;
    case kVK_ANSI_S: return ImGuiKey_S;
    case kVK_ANSI_D: return ImGuiKey_D;
    case kVK_ANSI_F: return ImGuiKey_F;
    case kVK_ANSI_H: return ImGuiKey_H;
    case kVK_ANSI_G: return ImGuiKey_G;
    case kVK_ANSI_Z: return ImGuiKey_Z;
    case kVK_ANSI_X: return ImGuiKey_X;
    case kVK_ANSI_C: return ImGuiKey_C;
    case kVK_ANSI_V: return ImGuiKey_V;
    case kVK_ANSI_B: return ImGuiKey_B;
    case kVK_ANSI_Q: return ImGuiKey_Q;
    case kVK_ANSI_W: return ImGuiKey_W;
    case kVK_ANSI_E: return ImGuiKey_E;
    case kVK_ANSI_R: return ImGuiKey_R;
    case kVK_ANSI_Y: return ImGuiKey_Y;
    case kVK_ANSI_T: return ImGuiKey_T;
    case kVK_ANSI_1: return ImGuiKey_1;
    case kVK_ANSI_2: return ImGuiKey_2;
    case kVK_ANSI_3: return ImGuiKey_3;
    case kVK_ANSI_4: return ImGuiKey_4;
    case kVK_ANSI_5: return ImGuiKey_5;
    case kVK_ANSI_6: return ImGuiKey_6;
    case kVK_ANSI_7: return ImGuiKey_7;
    case kVK_ANSI_8: return ImGuiKey_8;
    case kVK_ANSI_9: return ImGuiKey_9;
    case kVK_ANSI_0: return ImGuiKey_0;
    case kVK_ANSI_Equal: return ImGuiKey_Equal;
    case kVK_ANSI_Minus: return ImGuiKey_Minus;
    case kVK_ANSI_RightBracket: return ImGuiKey_RightBracket;
    case kVK_ANSI_LeftBracket: return ImGuiKey_LeftBracket;
    case kVK_ANSI_O: return ImGuiKey_O;
    case kVK_ANSI_U: return ImGuiKey_U;
    case kVK_ANSI_I: return ImGuiKey_I;
    case kVK_ANSI_P: return ImGuiKey_P;
    case kVK_ANSI_L: return ImGuiKey_L;
    case kVK_ANSI_J: return ImGuiKey_J;
    case kVK_ANSI_Quote: return ImGuiKey_Apostrophe;
    case kVK_ANSI_K: return ImGuiKey_K;
    case kVK_ANSI_Semicolon: return ImGuiKey_Semicolon;
    case kVK_ANSI_Backslash: return ImGuiKey_Backslash;
    case kVK_ANSI_Comma: return ImGuiKey_Comma;
    case kVK_ANSI_Slash: return ImGuiKey_Slash;
    case kVK_ANSI_N: return ImGuiKey_N;
    case kVK_ANSI_M: return ImGuiKey_M;
    case kVK_ANSI_Period: return ImGuiKey_Period;
    case kVK_ANSI_Grave: return ImGuiKey_GraveAccent;
    case kVK_ANSI_KeypadDecimal: return ImGuiKey_KeypadDecimal;
    case kVK_ANSI_KeypadMultiply: return ImGuiKey_KeypadMultiply;
    case kVK_ANSI_KeypadPlus: return ImGuiKey_KeypadAdd;
    case kVK_ANSI_KeypadClear: return ImGuiKey_NumLock;
    case kVK_ANSI_KeypadDivide: return ImGuiKey_KeypadDivide;
    case kVK_ANSI_KeypadEnter: return ImGuiKey_KeypadEnter;
    case kVK_ANSI_KeypadMinus: return ImGuiKey_KeypadSubtract;
    case kVK_ANSI_KeypadEquals: return ImGuiKey_KeypadEqual;
    case kVK_ANSI_Keypad0: return ImGuiKey_Keypad0;
    case kVK_ANSI_Keypad1: return ImGuiKey_Keypad1;
    case kVK_ANSI_Keypad2: return ImGuiKey_Keypad2;
    case kVK_ANSI_Keypad3: return ImGuiKey_Keypad3;
    case kVK_ANSI_Keypad4: return ImGuiKey_Keypad4;
    case kVK_ANSI_Keypad5: return ImGuiKey_Keypad5;
    case kVK_ANSI_Keypad6: return ImGuiKey_Keypad6;
    case kVK_ANSI_Keypad7: return ImGuiKey_Keypad7;
    case kVK_ANSI_Keypad8: return ImGuiKey_Keypad8;
    case kVK_ANSI_Keypad9: return ImGuiKey_Keypad9;
    case kVK_Return: return ImGuiKey_Enter;
    case kVK_Tab: return ImGuiKey_Tab;
    case kVK_Space: return ImGuiKey_Space;
    case kVK_Delete: return ImGuiKey_Backspace;
    case kVK_Escape: return ImGuiKey_Escape;
    case kVK_CapsLock: return ImGuiKey_CapsLock;
    case kVK_Control: return ImGuiKey_LeftCtrl;
    case kVK_Shift: return ImGuiKey_LeftShift;
    case kVK_Option: return ImGuiKey_LeftAlt;
    case kVK_Command: return ImGuiKey_LeftSuper;
    case kVK_RightControl: return ImGuiKey_RightCtrl;
    case kVK_RightShift: return ImGuiKey_RightShift;
    case kVK_RightOption: return ImGuiKey_RightAlt;
    case kVK_RightCommand: return ImGuiKey_RightSuper;
    case kVK_F1: return ImGuiKey_F1;
    case kVK_F2: return ImGuiKey_F2;
    case kVK_F3: return ImGuiKey_F3;
    case kVK_F4: return ImGuiKey_F4;
    case kVK_F5: return ImGuiKey_F5;
    case kVK_F6: return ImGuiKey_F6;
    case kVK_F7: return ImGuiKey_F7;
    case kVK_F8: return ImGuiKey_F8;
    case kVK_F9: return ImGuiKey_F9;
    case kVK_F10: return ImGuiKey_F10;
    case kVK_F11: return ImGuiKey_F11;
    case kVK_F12: return ImGuiKey_F12;
    case kVK_F13: return ImGuiKey_F13;
    case kVK_F14: return ImGuiKey_F14;
    case kVK_F15: return ImGuiKey_F15;
    case kVK_F16: return ImGuiKey_F16;
    case kVK_F17: return ImGuiKey_F17;
    case kVK_F18: return ImGuiKey_F18;
    case kVK_F19: return ImGuiKey_F19;
    case kVK_F20: return ImGuiKey_F20;
    case 0x6E: return ImGuiKey_Menu;
    case kVK_Help: return ImGuiKey_Insert;
    case kVK_Home: return ImGuiKey_Home;
    case kVK_PageUp: return ImGuiKey_PageUp;
    case kVK_ForwardDelete: return ImGuiKey_Delete;
    case kVK_End: return ImGuiKey_End;
    case kVK_PageDown: return ImGuiKey_PageDown;
    case kVK_LeftArrow: return ImGuiKey_LeftArrow;
    case kVK_RightArrow: return ImGuiKey_RightArrow;
    case kVK_DownArrow: return ImGuiKey_DownArrow;
    case kVK_UpArrow: return ImGuiKey_UpArrow;
    default: return ImGuiKey_None;
    }
}
//////////////////////////////////////////////////////////////////////////
+ (ImGuiMouseSource)mouseSourceForEvent:(NSEvent *)_event
{
    switch( [_event subtype] )
    {
    case NSEventSubtypeTabletPoint:
        return ImGuiMouseSource_Pen;
    case NSEventSubtypeMouseEvent:
    default:
        return ImGuiMouseSource_Mouse;
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)updateMouseCursor
{
    ImGuiIO & io = ImGui::GetIO();

    if( (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) != 0 )
    {
        return;
    }

    ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();

    if( io.MouseDrawCursor == true || imguiCursor == ImGuiMouseCursor_None )
    {
        if( m_mouseCursorHidden == NO )
        {
            m_mouseCursorHidden = YES;
            [NSCursor hide];
        }

        return;
    }

    NSCursor * desired = m_mouseCursors[imguiCursor] != nil ? m_mouseCursors[imguiCursor] : m_mouseCursors[ImGuiMouseCursor_Arrow];

    if( desired != [NSCursor currentCursor] )
    {
        [desired set];
    }

    if( m_mouseCursorHidden == YES )
    {
        m_mouseCursorHidden = NO;
        [NSCursor unhide];
    }
}
//////////////////////////////////////////////////////////////////////////
+ (void)updateGamepads
{
    ImGuiIO & io = ImGui::GetIO();

#if MENGINE_IMGUI_APPLE_HAS_CONTROLLER
    GCController * controller = [GCController current];
#else
    GCController * controller = [[GCController controllers] firstObject];
#endif

    if( controller == nil || [controller extendedGamepad] == nil )
    {
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
        return;
    }

    GCExtendedGamepad * gamepad = [controller extendedGamepad];

#define MENGINE_IMGUI_SATURATE(V) ((V) < 0.0f ? 0.0f : (V) > 1.0f ? 1.0f : (V))
#define MENGINE_IMGUI_MAP_BUTTON(KEY_NO, BUTTON_NAME) { io.AddKeyEvent( (KEY_NO), gamepad.BUTTON_NAME.isPressed ); }
#define MENGINE_IMGUI_MAP_ANALOG(KEY_NO, AXIS_NAME, V0, V1) { float vn = (float)(gamepad.AXIS_NAME.value - (V0)) / (float)((V1) - (V0)); vn = MENGINE_IMGUI_SATURATE( vn ); io.AddKeyAnalogEvent( (KEY_NO), vn > 0.1f, vn ); }

    const float thumbDeadZone = 0.0f;

#if MENGINE_IMGUI_APPLE_HAS_BUTTON_OPTIONS
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadBack, buttonOptions );
#endif
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadFaceLeft, buttonX );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadFaceRight, buttonB );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadFaceUp, buttonY );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadFaceDown, buttonA );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadDpadLeft, dpad.left );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadDpadRight, dpad.right );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadDpadUp, dpad.up );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadDpadDown, dpad.down );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadL1, leftShoulder, 0.0f, 1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadR1, rightShoulder, 0.0f, 1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadL2, leftTrigger, 0.0f, 1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadR2, rightTrigger, 0.0f, 1.0f );
#if MENGINE_IMGUI_APPLE_HAS_THUMBSTICKS
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadL3, leftThumbstickButton );
    MENGINE_IMGUI_MAP_BUTTON( ImGuiKey_GamepadR3, rightThumbstickButton );
#endif
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadLStickLeft, leftThumbstick.xAxis, -thumbDeadZone, -1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadLStickRight, leftThumbstick.xAxis, +thumbDeadZone, +1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadLStickUp, leftThumbstick.yAxis, +thumbDeadZone, +1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadLStickDown, leftThumbstick.yAxis, -thumbDeadZone, -1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadRStickLeft, rightThumbstick.xAxis, -thumbDeadZone, -1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadRStickRight, rightThumbstick.xAxis, +thumbDeadZone, +1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadRStickUp, rightThumbstick.yAxis, +thumbDeadZone, +1.0f );
    MENGINE_IMGUI_MAP_ANALOG( ImGuiKey_GamepadRStickDown, rightThumbstick.yAxis, -thumbDeadZone, -1.0f );

#undef MENGINE_IMGUI_MAP_ANALOG
#undef MENGINE_IMGUI_MAP_BUTTON
#undef MENGINE_IMGUI_SATURATE

    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
}
//////////////////////////////////////////////////////////////////////////
+ (void)addInputCharactersForEvent:(NSEvent *)_event
{
    NSEventModifierFlags flags = [_event modifierFlags];

    if( (flags & NSEventModifierFlagCommand) != 0 || (flags & NSEventModifierFlagControl) != 0 )
    {
        return;
    }

    NSString * characters = [_event characters];

    if( characters == nil || [characters length] == 0 )
    {
        return;
    }

    const char * text = [characters UTF8String];

    if( text == nullptr || text[0] == '\0' )
    {
        return;
    }

    ImGui::GetIO().AddInputCharactersUTF8( text );
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)initializeWithView:(NSView *)_view
{
    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    IMGUI_CHECKVERSION();
    IM_ASSERT( io.BackendPlatformUserData == nullptr && "Already initialized a platform backend" );

    io.BackendPlatformUserData = (__bridge void *)self;
    io.BackendPlatformName = "mengine_imgui_macos";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    m_window = [_view window] != nil ? [_view window] : [NSApp orderedWindows].firstObject;

    ImGuiViewport * mainViewport = ImGui::GetMainViewport();
    mainViewport->PlatformHandle = (__bridge void *)m_window;
    mainViewport->PlatformHandleRaw = (__bridge void *)m_window;

    m_mouseCursors[ImGuiMouseCursor_Arrow] = [NSCursor arrowCursor];
    m_mouseCursors[ImGuiMouseCursor_TextInput] = [NSCursor IBeamCursor];
    m_mouseCursors[ImGuiMouseCursor_ResizeAll] = [NSCursor closedHandCursor];
    m_mouseCursors[ImGuiMouseCursor_ResizeNS] = [NSCursor respondsToSelector:@selector(_windowResizeNorthSouthCursor)] ? [NSCursor _windowResizeNorthSouthCursor] : [NSCursor resizeUpDownCursor];
    m_mouseCursors[ImGuiMouseCursor_ResizeEW] = [NSCursor respondsToSelector:@selector(_windowResizeEastWestCursor)] ? [NSCursor _windowResizeEastWestCursor] : [NSCursor resizeLeftRightCursor];
    m_mouseCursors[ImGuiMouseCursor_ResizeNESW] = [NSCursor respondsToSelector:@selector(_windowResizeNorthEastSouthWestCursor)] ? [NSCursor _windowResizeNorthEastSouthWestCursor] : [NSCursor closedHandCursor];
    m_mouseCursors[ImGuiMouseCursor_ResizeNWSE] = [NSCursor respondsToSelector:@selector(_windowResizeNorthWestSouthEastCursor)] ? [NSCursor _windowResizeNorthWestSouthEastCursor] : [NSCursor closedHandCursor];
    m_mouseCursors[ImGuiMouseCursor_Hand] = [NSCursor pointingHandCursor];
    m_mouseCursors[ImGuiMouseCursor_Wait] = [NSCursor respondsToSelector:@selector(busyButClickableCursor)] ? [NSCursor busyButClickableCursor] : [NSCursor arrowCursor];
    m_mouseCursors[ImGuiMouseCursor_Progress] = m_mouseCursors[ImGuiMouseCursor_Wait];
    m_mouseCursors[ImGuiMouseCursor_NotAllowed] = [NSCursor operationNotAllowedCursor];

    platformIO.Platform_SetClipboardTextFn = []( ImGuiContext *, const char * _text )
    {
        PLATFORM_SERVICE()
            ->setClipboardText( _text );
    };

    platformIO.Platform_GetClipboardTextFn = []( ImGuiContext * ) -> const char *
    {
        MengineImGUIMacOSPlatform * platform = (__bridge MengineImGUIMacOSPlatform *)ImGui::GetIO().BackendPlatformUserData;

        if( platform == nil )
        {
            return nullptr;
        }

        return [platform clipboardText];
    };

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onApplicationBecomeActive:)
                                                 name:NSApplicationDidBecomeActiveNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onApplicationBecomeInactive:)
                                                 name:NSApplicationDidResignActiveNotification
                                               object:nil];

    return YES;
}
//////////////////////////////////////////////////////////////////////////
- (void)finalizeBackend
{
    if( [self isInitialized] == NO )
    {
        return;
    }

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    if( m_mouseCursorHidden == YES )
    {
        m_mouseCursorHidden = NO;
        [NSCursor unhide];
    }

    m_time = 0.0;
    m_window = nil;
    m_clipboard.clear();
    std::memset( m_mouseCursors, 0, sizeof( m_mouseCursors ) );

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();
    ImGuiViewport * mainViewport = ImGui::GetMainViewport();

    mainViewport->PlatformUserData = nullptr;
    mainViewport->PlatformHandle = nullptr;
    mainViewport->PlatformHandleRaw = nullptr;

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasGamepad);

    platformIO.ClearPlatformHandlers();
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)isInitialized
{
    if( ImGui::GetCurrentContext() == nullptr )
    {
        return NO;
    }

    const ImGuiIO & io = ImGui::GetIO();

    return io.BackendPlatformUserData == (__bridge void *)self;
}
//////////////////////////////////////////////////////////////////////////
- (void)newFrameWithView:(NSView *)_view
{
    IM_ASSERT( [self isInitialized] == YES && "ImGUIMacOSPlatform is not initialized" );

    ImGuiIO & io = ImGui::GetIO();

    if( _view != nil )
    {
        NSWindow * window = [_view window];
        const float fbScale = window != nil ? (float)[window backingScaleFactor] : 1.f;

        NSRect bounds = [_view bounds];
        io.DisplaySize = ImVec2( (float)bounds.size.width, (float)bounds.size.height );
        io.DisplayFramebufferScale = ImVec2( fbScale, fbScale );

        m_window = window;

        ImGuiViewport * mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformHandle = (__bridge void *)m_window;
        mainViewport->PlatformHandleRaw = (__bridge void *)m_window;
    }

    double currentTime = Mengine::Helper::getElapsedTime();

    if( m_time <= 0.0 )
    {
        io.DeltaTime = 1.f / 60.f;
    }
    else
    {
        io.DeltaTime = (float)(currentTime - m_time);
    }

    m_time = currentTime;

    [self updateMouseCursor];
    [MengineImGUIMacOSPlatform updateGamepads];
}
//////////////////////////////////////////////////////////////////////////
- (const char *)clipboardText
{
    NSPasteboard * pasteboard = [NSPasteboard generalPasteboard];
    NSString * string = [pasteboard stringForType:NSPasteboardTypeString];

    if( string == nil )
    {
        return nullptr;
    }

    const char * text = [string UTF8String];
    size_t textLen = std::strlen( text );
    m_clipboard.resize( (int)textLen + 1 );
    std::memcpy( m_clipboard.Data, text, textLen + 1 );

    return m_clipboard.Data;
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)handleEvent:(NSEvent *)_event view:(NSView *)_view
{
    if( _view == nil || _event == nil || [self isInitialized] == NO )
    {
        return NO;
    }

    NSWindow * viewWindow = [_view window];
    NSWindow * eventWindow = [_event window];

    if( eventWindow != nil && eventWindow != viewWindow )
    {
        return NO;
    }

    ImGuiIO & io = ImGui::GetIO();
    NSEventType eventType = [_event type];

    switch( eventType )
    {
    case NSEventTypeLeftMouseDown:
    case NSEventTypeRightMouseDown:
    case NSEventTypeOtherMouseDown:
    {
        int button = (int)[_event buttonNumber];

        if( button >= 0 && button < ImGuiMouseButton_COUNT )
        {
            io.AddMouseSourceEvent( [MengineImGUIMacOSPlatform mouseSourceForEvent:_event] );
            io.AddMouseButtonEvent( button, true );
        }

        return io.WantCaptureMouse;
    }
    case NSEventTypeLeftMouseUp:
    case NSEventTypeRightMouseUp:
    case NSEventTypeOtherMouseUp:
    {
        int button = (int)[_event buttonNumber];

        if( button >= 0 && button < ImGuiMouseButton_COUNT )
        {
            io.AddMouseSourceEvent( [MengineImGUIMacOSPlatform mouseSourceForEvent:_event] );
            io.AddMouseButtonEvent( button, false );
        }

        return io.WantCaptureMouse;
    }
    case NSEventTypeMouseMoved:
    case NSEventTypeLeftMouseDragged:
    case NSEventTypeRightMouseDragged:
    case NSEventTypeOtherMouseDragged:
    {
        NSPoint mousePoint = [_event locationInWindow];

        if( eventWindow == nil && viewWindow != nil )
        {
            mousePoint = [viewWindow convertPointFromScreen:mousePoint];
        }

        mousePoint = [_view convertPoint:mousePoint fromView:nil];

        if( [_view isFlipped] == NO )
        {
            mousePoint = NSMakePoint( mousePoint.x, [_view bounds].size.height - mousePoint.y );
        }

        io.AddMouseSourceEvent( [MengineImGUIMacOSPlatform mouseSourceForEvent:_event] );
        io.AddMousePosEvent( (float)mousePoint.x, (float)mousePoint.y );

        return io.WantCaptureMouse;
    }
    case NSEventTypeScrollWheel:
    {
        if( [_event phase] == NSEventPhaseCancelled )
        {
            return NO;
        }

        double wheelDx = [_event scrollingDeltaX];
        double wheelDy = [_event scrollingDeltaY];

        if( [_event hasPreciseScrollingDeltas] == YES )
        {
            wheelDx *= 0.01;
            wheelDy *= 0.01;
        }

        if( wheelDx != 0.0 || wheelDy != 0.0 )
        {
            io.AddMouseWheelEvent( (float)wheelDx, (float)wheelDy );
        }

        return io.WantCaptureMouse;
    }
    case NSEventTypeKeyDown:
    case NSEventTypeKeyUp:
    {
        if( [_event isARepeat] == YES )
        {
            return io.WantCaptureKeyboard;
        }

        int keyCode = (int)[_event keyCode];
        ImGuiKey key = [MengineImGUIMacOSPlatform keyCodeToImGuiKey:keyCode];
        bool down = eventType == NSEventTypeKeyDown;

        io.AddKeyEvent( key, down );
        io.SetKeyEventNativeData( key, keyCode, -1 );

        if( down == true )
        {
            [MengineImGUIMacOSPlatform addInputCharactersForEvent:_event];
        }

        return io.WantCaptureKeyboard;
    }
    case NSEventTypeFlagsChanged:
    {
        unsigned short keyCode = [_event keyCode];
        NSEventModifierFlags flags = [_event modifierFlags];

        io.AddKeyEvent( ImGuiMod_Shift, (flags & NSEventModifierFlagShift) != 0 );
        io.AddKeyEvent( ImGuiMod_Ctrl, (flags & NSEventModifierFlagControl) != 0 );
        io.AddKeyEvent( ImGuiMod_Alt, (flags & NSEventModifierFlagOption) != 0 );
        io.AddKeyEvent( ImGuiMod_Super, (flags & NSEventModifierFlagCommand) != 0 );

        ImGuiKey key = [MengineImGUIMacOSPlatform keyCodeToImGuiKey:keyCode];

        if( key != ImGuiKey_None )
        {
            NSEventModifierFlags mask = 0;

            switch( key )
            {
            case ImGuiKey_LeftCtrl: mask = 0x0001; break;
            case ImGuiKey_RightCtrl: mask = 0x2000; break;
            case ImGuiKey_LeftShift: mask = 0x0002; break;
            case ImGuiKey_RightShift: mask = 0x0004; break;
            case ImGuiKey_LeftSuper: mask = 0x0008; break;
            case ImGuiKey_RightSuper: mask = 0x0010; break;
            case ImGuiKey_LeftAlt: mask = 0x0020; break;
            case ImGuiKey_RightAlt: mask = 0x0040; break;
            default:
                return io.WantCaptureKeyboard;
            }

            io.AddKeyEvent( key, (flags & mask) != 0 );
            io.SetKeyEventNativeData( key, keyCode, -1 );
        }

        return io.WantCaptureKeyboard;
    }
    default:
        return NO;
    }
}
//////////////////////////////////////////////////////////////////////////

- (void)onApplicationBecomeActive:(NSNotification *)_notification
{
    (void)_notification;

    ImGui::GetIO().AddFocusEvent( true );
}
//////////////////////////////////////////////////////////////////////////
- (void)onApplicationBecomeInactive:(NSNotification *)_notification
{
    (void)_notification;

    ImGui::GetIO().AddFocusEvent( false );
}
//////////////////////////////////////////////////////////////////////////
@end
