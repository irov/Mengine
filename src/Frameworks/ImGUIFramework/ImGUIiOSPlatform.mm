#include "ImGUIiOSPlatform.h"

#include "Interface/PlatformServiceInterface.h"

#include "Kernel/TimestampHelper.h"

#include "imgui.h"

#import <dispatch/dispatch.h>

#include <cstring>

@interface MengineImGUIiOSPlatform()
{
    double m_time;
    UITouch * m_touches[ImGuiMouseButton_COUNT];
    BOOL m_touchCaptured[ImGuiMouseButton_COUNT];
    ImVector<char> m_clipboard;
}

- (BOOL)initializeWithView:(UIView *)_view;
- (void)finalizeBackend;
- (BOOL)isInitialized;
- (void)newFrameWithView:(UIView *)_view;
- (BOOL)handleTouches:(NSSet<UITouch *> *)_touches view:(UIView *)_view phase:(UITouchPhase)_phase;
- (const char *)clipboardText;
- (int)acquireTouch:(UITouch *)_touch;
- (int)getTouch:(UITouch *)_touch;
- (int)releaseTouch:(UITouch *)_touch;
- (void)addTouchPositionEvent:(UITouch *)_touch view:(UIView *)_view io:(ImGuiIO &)_io;

@end

//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIiOSPlatform

+ (instancetype)sharedInstance
{
    static MengineImGUIiOSPlatform * instance = nil;
    static dispatch_once_t onceToken;

    dispatch_once( &onceToken, ^{
        instance = [[MengineImGUIiOSPlatform alloc] init];
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
    std::memset( m_touches, 0, sizeof( m_touches ) );
    std::memset( m_touchCaptured, 0, sizeof( m_touchCaptured ) );

    return self;
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)initializeWithView:(UIView *)_view
{
    return [[MengineImGUIiOSPlatform sharedInstance] initializeWithView:_view];
}
//////////////////////////////////////////////////////////////////////////
+ (void)finalizeBackend
{
    [[MengineImGUIiOSPlatform sharedInstance] finalizeBackend];
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)isInitialized
{
    return [[MengineImGUIiOSPlatform sharedInstance] isInitialized];
}
//////////////////////////////////////////////////////////////////////////
+ (void)newFrameWithView:(UIView *)_view
{
    [[MengineImGUIiOSPlatform sharedInstance] newFrameWithView:_view];
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)handleTouches:(NSSet<UITouch *> *)_touches view:(UIView *)_view phase:(UITouchPhase)_phase
{
    return [[MengineImGUIiOSPlatform sharedInstance] handleTouches:_touches view:_view phase:_phase];
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)initializeWithView:(UIView *)_view
{
    (void)_view;

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    IMGUI_CHECKVERSION();
    IM_ASSERT( io.BackendPlatformUserData == nullptr && "Already initialized a platform backend" );

    io.BackendPlatformUserData = (__bridge void *)self;
    io.BackendPlatformName = "mengine_imgui_ios";

    platformIO.Platform_SetClipboardTextFn = []( ImGuiContext *, const char * _text )
    {
        PLATFORM_SERVICE()
            ->setClipboardText( _text );
    };

    platformIO.Platform_GetClipboardTextFn = []( ImGuiContext * ) -> const char *
    {
        MengineImGUIiOSPlatform * platform = (__bridge MengineImGUIiOSPlatform *)ImGui::GetIO().BackendPlatformUserData;

        if( platform == nil )
        {
            return nullptr;
        }

        return [platform clipboardText];
    };

    return YES;
}
//////////////////////////////////////////////////////////////////////////
- (void)finalizeBackend
{
    if( [self isInitialized] == NO )
    {
        return;
    }

    m_time = 0.0;
    m_clipboard.clear();
    std::memset( m_touches, 0, sizeof( m_touches ) );
    std::memset( m_touchCaptured, 0, sizeof( m_touchCaptured ) );

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;

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
- (void)newFrameWithView:(UIView *)_view
{
    IM_ASSERT( [self isInitialized] == YES && "ImGUIiOSPlatform is not initialized" );

    ImGuiIO & io = ImGui::GetIO();

    if( _view != nil )
    {
        CGRect bounds = [_view bounds];
        CGFloat scale = [_view contentScaleFactor];

        if( scale <= 0.f )
        {
            scale = [UIScreen mainScreen].scale;
        }

        io.DisplaySize = ImVec2( (float)bounds.size.width, (float)bounds.size.height );
        io.DisplayFramebufferScale = ImVec2( (float)scale, (float)scale );
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
}
//////////////////////////////////////////////////////////////////////////
- (const char *)clipboardText
{
    UIPasteboard * pasteboard = [UIPasteboard generalPasteboard];
    NSString * string = pasteboard.string;

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
- (int)acquireTouch:(UITouch *)_touch
{
    for( int index = 0; index != ImGuiMouseButton_COUNT; ++index )
    {
        if( m_touches[index] == _touch )
        {
            return index;
        }
    }

    for( int index = 0; index != ImGuiMouseButton_COUNT; ++index )
    {
        if( m_touches[index] == nil )
        {
            m_touches[index] = _touch;

            return index;
        }
    }

    return -1;
}
//////////////////////////////////////////////////////////////////////////
- (int)getTouch:(UITouch *)_touch
{
    for( int index = 0; index != ImGuiMouseButton_COUNT; ++index )
    {
        if( m_touches[index] == _touch )
        {
            return index;
        }
    }

    return -1;
}
//////////////////////////////////////////////////////////////////////////
- (int)releaseTouch:(UITouch *)_touch
{
    int index = [self getTouch:_touch];

    if( index == -1 )
    {
        return -1;
    }

    m_touches[index] = nil;
    m_touchCaptured[index] = NO;

    return index;
}
//////////////////////////////////////////////////////////////////////////
- (void)addTouchPositionEvent:(UITouch *)_touch view:(UIView *)_view io:(ImGuiIO &)_io
{
    CGPoint location = [_touch locationInView:_view];

    _io.AddMouseSourceEvent( ImGuiMouseSource_TouchScreen );
    _io.AddMousePosEvent( (float)location.x, (float)location.y );
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)handleTouches:(NSSet<UITouch *> *)_touches view:(UIView *)_view phase:(UITouchPhase)_phase
{
    if( _view == nil || _touches == nil || [self isInitialized] == NO )
    {
        return NO;
    }

    ImGuiIO & io = ImGui::GetIO();
    BOOL handled = NO;

    for( UITouch * touch in _touches )
    {
        switch( _phase )
        {
        case UITouchPhaseBegan:
        {
            int button = [self acquireTouch:touch];

            if( button == -1 )
            {
                continue;
            }

            [self addTouchPositionEvent:touch view:_view io:io];
            io.AddMouseButtonEvent( button, true );

            m_touchCaptured[button] = io.WantCaptureMouse ? YES : NO;
            handled = handled || m_touchCaptured[button];
        } break;
        case UITouchPhaseMoved:
        case UITouchPhaseStationary:
        {
            int button = [self getTouch:touch];

            if( button == -1 )
            {
                continue;
            }

            [self addTouchPositionEvent:touch view:_view io:io];

            handled = handled || m_touchCaptured[button];
        } break;
        case UITouchPhaseEnded:
        case UITouchPhaseCancelled:
        {
            int button = [self getTouch:touch];

            if( button == -1 )
            {
                continue;
            }

            BOOL captured = m_touchCaptured[button];
            [self addTouchPositionEvent:touch view:_view io:io];
            io.AddMouseButtonEvent( button, false );
            [self releaseTouch:touch];

            handled = handled || captured;
        } break;
        default:
            break;
        }
    }

    return handled;
}
//////////////////////////////////////////////////////////////////////////
@end
