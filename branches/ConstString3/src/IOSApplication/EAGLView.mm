//
//  EAGLView.m
//  Mengine
//
//  Created by Dgut on 9/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "EAGLView.h"
#import "MengineAppDelegate.h"

#import "ES1Renderer.h"

#import "Menge/Application.h"

EAGLView * pEAGLView = 0;

extern Menge::iOSApplication * pApplication;


@interface EAGLView ()
@property (nonatomic, getter=isAnimating) BOOL animating;
@end



@implementation EAGLView

@synthesize animating, animationFrameInterval, displayLink, animationTimer;


// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
    self = [super initWithCoder:coder];
    if (self)
    {
		pEAGLView = self;
		
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

		renderer = [[ES1Renderer alloc] init];

		if (!renderer)
		{
			[self release];
			return nil;
		}

        animating = FALSE;
        displayLinkSupported = FALSE;
        animationFrameInterval = 1;
        displayLink = nil;
        animationTimer = nil;

        // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
        // class is used as fallback when it isn't available.
        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
            displayLinkSupported = TRUE;
    }

    return self;
}

- (void)drawView:(id)sender
{	
	[renderer render];
}

- (void)layoutSubviews
{
    [renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;

        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        if (displayLinkSupported)
        {
            // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
            // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
            // not be called in system versions earlier than 3.1.

            self.displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
            [displayLink setFrameInterval:animationFrameInterval];
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
        else
            self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];

        self.animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            self.displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            self.animationTimer = nil;
        }

        self.animating = FALSE;
    }
}

- (void)dealloc
{
    [renderer release];
    renderer = nil;
    
    [displayLink release];
    displayLink = nil;

    [super dealloc];
}

mt::vec2f old[ 16 ];

//bool key = false;

- ( void )touchesBegan : ( NSSet * )touches withEvent : ( UIEvent * )event
{
    //NSLog( @"touchesBegan" );
	/*if( key )
		[ self hideKeyboard ];
	else
		[ self showKeyboard ];
	
	key = !key;*/
	
	int i = 0;
	
	for( UITouch * touch in touches )
	{
		CGPoint point = [ touch locationInView : nil ];
		
		mt::vec2f p;
		
		p.x = point.y;
		p.y = 768 - point.x;
		
		old[ i ] = p;
		
		//NSLog( @"down x=%i y=%i", ( int )p.x, ( int )p.y );
		
        if( pApplication != 0 )
        {
            Menge::ApplicationInterface * app = pApplication->getApplication();
            
            app->setCursorPosition( p );
            app->pushMouseButtonEvent( p, i, true );
        }
		
		i++;
	}
}

- ( void )touchesMoved : ( NSSet * )touches withEvent : ( UIEvent * )event
{
    //NSLog( @"touchesMoved" );
    
	int i = 0;
	
	for( UITouch * touch in touches )
	{
		CGPoint point = [ touch locationInView : nil ];
		
		mt::vec2f p;
		
		p.x = point.y;
		p.y = 768 - point.x;
		
		//NSLog( @"move x=%i y=%i dx=%i dy=%i", ( int )p.x, ( int )p.y, int( p.x - old[ i ].x ), int( p.y - old[ i ].y ) );
		
        Menge::ApplicationInterface * app = pApplication->getApplication();
        
        if( pApplication != 0 )
        {
            app->setCursorPosition( p );
            app->pushMouseMoveEvent( p, p.x - old[ i ].x, p.y - old[ i ].y, 0 );
        }
		
		old[ i ] = p;
		
		i++;
	}
    //NSLog( @"touchesMoved2" );
}

- ( void )touchesEnded : ( NSSet * )touches withEvent : ( UIEvent * )event
{
    //NSLog( @"touchesEnded" );
    
	int i = 0;
	
	for( UITouch * touch in touches )
	{
		CGPoint point = [ touch locationInView : nil ];
		
		mt::vec2f p;
		
		p.x = point.y;
		p.y = ( 768 - point.x );
		
		//NSLog( @"up   x=%i y=%i", ( int )p.x, ( int )p.y );
		
        if( pApplication != 0 )
        {
            Menge::ApplicationInterface * app = pApplication->getApplication();
            
            app->setCursorPosition( p );
            app->pushMouseButtonEvent( p, i, false );
        }
		
		i++;
	}
}

- ( void )showKeyboard
{
    //NSLog( @"showKeyboard" );
    
	[ self becomeFirstResponder ];
}

- ( void )hideKeyboard
{
    //NSLog( @"hideKeyboard" );
    
	[ self resignFirstResponder ];
}

- (BOOL)canBecomeFirstResponder
{
	return YES;
}

- (void)deleteBackward
{
    //NSLog( @"deleteBackward" );
    
    if( pApplication != 0 )
    {
        Menge::ApplicationInterface * app = pApplication->getApplication();
        
        app->pushKeyEvent( old[ 0 ], 0x08, 0x08, true );
        app->pushKeyEvent( old[ 0 ], 0x08, 0x08, false );
    }
}

- (BOOL)hasText
{
	return YES;
}

unsigned int GetMSVirtualKey( unsigned int symbol )
{
	if( symbol >= 'a' && symbol <= 'z' )
		return 0x41 + symbol - 'a';
	
	if( symbol >= 'A' && symbol <= 'Z' )
		return 0x41 + symbol - 'A';
	
	if( symbol >= '0' && symbol <= '9' )
		return 0x30 + symbol - '0';
	
	if( symbol == '\n' )
		return 0x0D;
	
	return 0;
}

- (void)insertText:(NSString *)text
{
    //NSLog( @"insertText" );
    
	unsigned int symbol = 0;
	
	const char * str = [ text cStringUsingEncoding : NSUnicodeStringEncoding ];
    
	if( str )
    {
		for( int i = 0; *str; i++, str++ )
        {
			symbol |= ( ( unsigned int )( *str ) ) << ( i * 8 );
        }
    }
	
    if( pApplication != 0 )
    {
        Menge::ApplicationInterface * app = pApplication->getApplication();
        
        app->pushKeyEvent( old[ 0 ], GetMSVirtualKey( symbol ), symbol, true );
        app->pushKeyEvent( old[ 0 ], GetMSVirtualKey( symbol ), symbol, false );
    }
}

/*- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	Menge::Application::get()->pushKeyEvent( old[ 0 ], 0x0D, 0x0D, true );
	Menge::Application::get()->pushKeyEvent( old[ 0 ], 0x0D, 0x0D, false );
	
	return NO;
}*/

@end
