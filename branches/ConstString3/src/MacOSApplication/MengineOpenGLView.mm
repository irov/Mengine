//
//  MengineOpenGLView.m
//  Mengine
//
//  Created by Dgut on 26.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "MengineOpenGLView.h"
#import "AppDelegate.h"
#import "Application.h"
#import <Carbon/Carbon.h>
#import <OpenGL/gl.h>

MengineOpenGLView * pMengineOpenGLView = nil;

@implementation MengineOpenGLView

@synthesize cursor;

//-( id )init
- (id)initWithCoder:(NSCoder *)coder
{
    //self = [ super init ];
    self = [ super initWithCoder:(NSCoder *)coder ];
    if( self )
    {
        [ [ self window ] makeFirstResponder:self ];
        //[ [ self window ] setAcceptsMouseMovedEvents : YES ];
        pMengineOpenGLView = self;
        cursor = nil;
    }
    
    return self;
}

- (void) viewWillMoveToWindow:(NSWindow *)newWindow {
    // Setup a new tracking area when the view is added to the window.
    /*NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[self frame] options: (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways) owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];*/
}

- (void)updateTrackingAreas
{
}

/*static void drawAnObject( void )
{
    //glColor3f( r, g, b );
    
    glBegin( GL_TRIANGLES );
    {
        glVertex3f( 0.f, 0.6f, 0.f );
        glVertex3f( -0.2f, -0.3f, 0.f );
        glVertex3f( 0.2f, -0.3f, 0.f );
    }
    glEnd();
}*/

-( void )drawRect: ( NSRect )dirtyRect
{  
    /*glClearColor( ( float )rand() / RAND_MAX, ( float )rand() / RAND_MAX, ( float )rand() / RAND_MAX, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    /*drawAnObject();*/
    //glFlush();
    
    //glViewport();
    glClear( GL_COLOR_BUFFER_BIT );
    
    if( pApplication )
        pApplication->Frame();
    
    //NSLog( @"[ [ self openGLContext ] flushBuffer ];" );
    
    [ [ self openGLContext ] flushBuffer ];
}

-( void )reshape
{
    /*if( [ self trackingAreas ].count )    
        [ self removeTrackingArea: [ [ self trackingAreas ] objectAtIndex : 0 ] ];
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[self frame] options: (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways) owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];*/
    
    /*if( pApplication )
        pApplication->FocusCursor( true );*/
}

static mt::vec2f old( -100, -100 );

static bool IsInside( NSPoint p, NSRect rect )
{
    return p.x >= rect.origin.x && p.x <= rect.origin.x + rect.size.width && p.y >= rect.origin.y && p.y <= rect.origin.y + rect.size.height;
}

-( void )updateCursor
{
    NSPoint p = [ NSEvent mouseLocation ];
    NSRect rect = [ self.window frame ];
    
    //NSLog( @"%f %f", p.x, p.y );
    NSLog( @"%f %f", rect.origin.x, rect.origin.y );
    
    if( cursor != nil )
        if( IsInside( p, rect ) )
        {
            [ cursor set ];
        }
        else
        {
            [ [ NSCursor arrowCursor ] set ];
        }
}

-( void )move:(NSEvent *)theEvent
{
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
    
    if( old.x == -100 )
        old = point;
    
    //NSLog( @"Move %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseMoveEvent( point, point.x - old.x, point.y - old.y, 0 );								
    Menge::Application::get()->setCursorPosition( point );
    
    //NSLog( @"%f %f", p.x, p.y );  
    
    //[ self updateCursor ];
    
    old = point;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    //NSLog( @"move" );
    [ self move : theEvent ];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    [ self move : theEvent ];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    [ self move : theEvent ];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
    [ self move : theEvent ];
}

- (void)mouseDown:(NSEvent *)theEvent
{    
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
        
    //NSLog( @"Left down %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 0, true );
}

- (void)mouseUp:(NSEvent *)theEvent
{    
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - point.y;
    
    //NSLog( @"Left up %f %f", point.x, self.frame.size.height - point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 0, false );
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
    
    //NSLog( @"Right down %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 1, true );
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
    
    //NSLog( @"Right up %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 1, false );
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
    
    //NSLog( @"Other down %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 2, true );
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    mt::vec2f point;
    
    NSPoint p = [ theEvent locationInWindow ];
    
    point.x = p.x;
    point.y = self.frame.size.height - p.y;
    
    //NSLog( @"Other up %f %f", point.x, point.y );
    
    Menge::Application::get()->pushMouseButtonEvent( point, 2, false );
}

static bool cmd = false;
static bool ctrl = false;
static bool alt = false;
static bool shift = false;

- (void)flagsChanged:(NSEvent *)theEvent
{    
    NSUInteger flags = [ theEvent modifierFlags ];
    
    bool cmd_ = flags & NSCommandKeyMask;
    bool ctrl_ = flags & NSControlKeyMask;
    bool alt_ = flags & NSAlternateKeyMask;
    bool shift_ = flags & NSShiftKeyMask;
    
    if( cmd != cmd_ )
        Menge::Application::get()->pushKeyEvent( old, 0x5D /*Applications key?*/, 0, cmd_ );
    if( ctrl != ctrl_ )
        Menge::Application::get()->pushKeyEvent( old, 0x11 /*CTRL?*/, 0, ctrl_ );
    if( alt != alt_ )
        Menge::Application::get()->pushKeyEvent( old, 0x12 /*ALT?*/, 0, alt_ );
    if( shift != shift_ )
        Menge::Application::get()->pushKeyEvent( old, 0x10 /*SHIFT?*/, 0, shift_ );
    
    cmd = cmd_;
    ctrl = ctrl_;
    alt = alt_;
    shift = shift_;
}

#define VK_LBUTTON      1
#define VK_RBUTTON      2
#define VK_CANCEL       3
#define VK_MBUTTON      4
#define VK_BACK         8
#define VK_TAB          9
#define VK_CLEAR        12
#define VK_RETURN       13
#define VK_SHIFT        16
#define VK_CONTROL      17
#define VK_MENU         18
#define VK_PAUSE        19
#define VK_CAPITAL      20
#define VK_ESCAPE       0x1B
#define VK_SPACE        32
#define VK_PRIOR        33
#define VK_NEXT         34
#define VK_END          35
#define VK_HOME         36
#define VK_LEFT         37
#define VK_UP           38
#define VK_RIGHT        39
#define VK_DOWN         40
#define VK_SELECT       41
#define VK_PRINT        42
#define VK_EXECUTE      43
#define VK_SNAPSHOT     44
#define VK_INSERT       45
#define VK_DELETE       46
#define VK_HELP         47
#define VK_LWIN         0x5B
#define VK_RWIN         0x5C
#define VK_APPS         0x5D
#define VK_NUMPAD0      0x60
#define VK_NUMPAD1      0x61
#define VK_NUMPAD2      0x62
#define VK_NUMPAD3      0x63
#define VK_NUMPAD4      0x64
#define VK_NUMPAD5      0x65
#define VK_NUMPAD6      0x66
#define VK_NUMPAD7      0x67
#define VK_NUMPAD8      0x68
#define VK_NUMPAD9      0x69
#define VK_MULTIPLY     0x6A
#define VK_ADD          0x6B
#define VK_SEPARATOR	0x6C
#define VK_SUBTRACT     0x6D
#define VK_DECIMAL      0x6E
#define VK_DIVIDE       0x6F
#define VK_F1           0x70
#define VK_F2           0x71
#define VK_F3           0x72
#define VK_F4           0x73
#define VK_F5           0x74
#define VK_F6           0x75
#define VK_F7           0x76
#define VK_F8           0x77
#define VK_F9           0x78
#define VK_F10          0x79
#define VK_F11          0x7A
#define VK_F12          0x7B
#define VK_F13          0x7C
#define VK_F14          0x7D
#define VK_F15          0x7E
#define VK_F16          0x7F
#define VK_F17          0x80
#define VK_F18          0x81
#define VK_F19          0x82
#define VK_F20          0x83
#define VK_F21          0x84
#define VK_F22          0x85
#define VK_F23          0x86
#define VK_F24          0x87
#define VK_NUMLOCK      0x90
#define VK_SCROLL       0x91
#define VK_LSHIFT       0xA0
#define VK_RSHIFT       0xA1
#define VK_LCONTROL     0xA2
#define VK_RCONTROL     0xA3
#define VK_LMENU        0xA4
#define VK_RMENU        0xA5
#define VK_PROCESSKEY	0xE5
#define VK_ATTN         0xF6
#define VK_CRSEL        0xF7
#define VK_EXSEL        0xF8
#define VK_EREOF        0xF9
#define VK_PLAY         0xFA
#define VK_ZOOM         0xFB   
#define VK_NONAME       0xFC
#define VK_PA1          0xFD
#define VK_OEM_CLEAR	0xFE

unsigned short VirtualKeyFromMacOXSToWin( unsigned short key )
{
#define TO( x, y ) case x: return y;
    
    switch( key )
    {
            TO( kVK_Delete, VK_BACK )
            TO( kVK_Tab, VK_TAB )
            TO( kVK_Return, VK_RETURN )
            TO( kVK_F15, VK_PAUSE )
            TO( kVK_CapsLock, VK_CAPITAL )
            TO( kVK_Escape, VK_ESCAPE )
            TO( kVK_Space, VK_SPACE )
            TO( kVK_PageUp, VK_PRIOR )
            TO( kVK_PageDown, VK_NEXT )
            TO( kVK_End, VK_END )
            TO( kVK_Home, VK_HOME )
            TO( kVK_LeftArrow, VK_LEFT )
            TO( kVK_UpArrow, VK_UP )
            TO( kVK_RightArrow, VK_RIGHT )
            TO( kVK_DownArrow, VK_DOWN )
            TO( kVK_F13, VK_SNAPSHOT )
            TO( kVK_Help, VK_INSERT )
            TO( kVK_ForwardDelete, VK_DELETE )
            TO( kVK_ANSI_0, 0x30 )
            TO( kVK_ANSI_1, 0x31 )
            TO( kVK_ANSI_2, 0x32 )
            TO( kVK_ANSI_3, 0x33 )
            TO( kVK_ANSI_4, 0x34 )
            TO( kVK_ANSI_5, 0x35 )
            TO( kVK_ANSI_6, 0x36 )
            TO( kVK_ANSI_7, 0x37 )
            TO( kVK_ANSI_8, 0x38 )
            TO( kVK_ANSI_9, 0x39 )
            TO( kVK_ANSI_A, 0x41 )
            TO( kVK_ANSI_B, 0x42 )
            TO( kVK_ANSI_C, 0x43 )
            TO( kVK_ANSI_D, 0x44 )
            TO( kVK_ANSI_E, 0x45 )
            TO( kVK_ANSI_F, 0x46 )
            TO( kVK_ANSI_G, 0x47 )
            TO( kVK_ANSI_H, 0x48 )
            TO( kVK_ANSI_I, 0x49 )
            TO( kVK_ANSI_J, 0x4A )
            TO( kVK_ANSI_K, 0x4B )
            TO( kVK_ANSI_L, 0x4C )
            TO( kVK_ANSI_M, 0x4D )
            TO( kVK_ANSI_N, 0x4E )
            TO( kVK_ANSI_O, 0x4F )
            TO( kVK_ANSI_P, 0x50 )
            TO( kVK_ANSI_Q, 0x51 )
            TO( kVK_ANSI_R, 0x52 )
            TO( kVK_ANSI_S, 0x53 )
            TO( kVK_ANSI_T, 0x54 )
            TO( kVK_ANSI_U, 0x55 )
            TO( kVK_ANSI_V, 0x56 )
            TO( kVK_ANSI_W, 0x57 )
            TO( kVK_ANSI_X, 0x58 )
            TO( kVK_ANSI_Y, 0x59 )
            TO( kVK_ANSI_Z, 0x5A )
            TO( kVK_ANSI_Keypad0, VK_NUMPAD0 )
            TO( kVK_ANSI_Keypad1, VK_NUMPAD1 )
            TO( kVK_ANSI_Keypad2, VK_NUMPAD2 )
            TO( kVK_ANSI_Keypad3, VK_NUMPAD3 )
            TO( kVK_ANSI_Keypad4, VK_NUMPAD4 )
            TO( kVK_ANSI_Keypad5, VK_NUMPAD5 )
            TO( kVK_ANSI_Keypad6, VK_NUMPAD6 )
            TO( kVK_ANSI_Keypad7, VK_NUMPAD7 )
            TO( kVK_ANSI_Keypad8, VK_NUMPAD8 )
            TO( kVK_ANSI_Keypad9, VK_NUMPAD9 )
            TO( kVK_ANSI_KeypadMultiply, VK_MULTIPLY )
            TO( kVK_ANSI_KeypadPlus, VK_ADD )
            TO( kVK_ANSI_KeypadDecimal, VK_SEPARATOR )
            TO( kVK_ANSI_KeypadMinus, VK_SUBTRACT )
            TO( kVK_ANSI_KeypadDivide, VK_DIVIDE )
            TO( kVK_F1, VK_F1 )
            TO( kVK_F2, VK_F2 )
            TO( kVK_F3, VK_F3 )
            TO( kVK_F4, VK_F4 )
            TO( kVK_F5, VK_F5 )
            TO( kVK_F6, VK_F6 )
            TO( kVK_F7, VK_F7 )
            TO( kVK_F8, VK_F8 )
    }
    
    return 0;
}

- (void)keyDown:(NSEvent *)event
{
    /*NSString * str = [ event charactersIgnoringModifiers ];
     unichar ch = [ str characterAtIndex : 0 ];*/
    unsigned short key = [ event keyCode ];
    unsigned short ch = [ [ event characters ] characterAtIndex : 0 ];
    //const char * c = [ [ event characters ] cStringUsingEncoding : NSUTF16StringEncoding ];
    
    NSLog( @"char=%04X", ch );
    
#define CASE( x, y ) case x: ch = y; break;
    
    if( ch == 0x0D && alt )
        Menge::Application::get()->setFullscreenMode( !Menge::Application::get()->getFullscreenMode() );

    switch( ch )
    {
            CASE( 0x7F, 0x08 )  // backspace
            CASE( 0xF700, 0 )   // up
            CASE( 0xF701, 0 )   // down
            CASE( 0xF702, 0 )   // left
            CASE( 0xF703, 0 )   // right
            CASE( 0x18, 0x1B )   // escape
            CASE( 0xF72B, 0 )   // end
            CASE( 0xF729, 0 )   // home
            CASE( 0xF72C, 0 )   // page up
            CASE( 0xF72D, 0 )   // page down
            CASE( 0xF728, 0x7F )   // delete
            CASE( 0xF739, 0 )   // num lock
    }
    
    Menge::Application::get()->pushKeyEvent( old, VirtualKeyFromMacOXSToWin( key ), ch, true );
}

- (void)keyUp:(NSEvent *)event
{
    /*NSString * str = [ event charactersIgnoringModifiers ];
     unichar ch = [ str characterAtIndex : 0 ];*/
    unsigned short key = [ event keyCode ];
    //unsigned short ch = [ [ event characters ] characterAtIndex : 0 ];
    
    Menge::Application::get()->pushKeyEvent( old, VirtualKeyFromMacOXSToWin( key ), 0, false );
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    //NSLog( @"wheel %f", [ theEvent deltaY ] );
    
    Menge::Application::get()->pushMouseMoveEvent( old, 0, 0, [ theEvent deltaY ] );
}

/*- (void)mouseEntered:(NSEvent *)theEvent
{
    pApplication->FocusCursor(true);
}

- (void)mouseExited:(NSEvent *)theEvent
{
    pApplication->FocusCursor(false);
}*/

/*- (void) resetCursorRects
{
    [super resetCursorRects];
    [ self discardCursorRects ];
    if( cursor != nil )
        [self addCursorRect: [self bounds] cursor: cursor];
    
    NSLog( @"resetCursorRects" );
}*/

@end
