/*
 *  MacOSXApplication.cpp
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MacOSXApplication.h"
#include "Menge/Application.h"
#include "Menge/FileEngine.h"
#include "MengineOpenGLView.h"
#include "AppDelegate.h"
#include <sys/stat.h>
#include <sys/time.h>

namespace Menge
{

void MacOSXLogger::log( const void * _data, int _count, EMessageLevel _level )
{
}

void MacOSXLogger::flush( void )
{
}

void MacOSXLogSystem::setVerboseLevel( EMessageLevel _level )
{
}

void MacOSXLogSystem::logMessage( const String & _message, EMessageLevel _level )
{
	NSLog( @"%s", _message.c_str() );
}
	
bool MacOSXLogSystem::registerLogger( LoggerInterface * _logger )
{
	return true;
}
	
void MacOSXLogSystem::unregisterLogger( LoggerInterface * _logger )
{
}
	
MacOSXTimer::MacOSXTimer( void ) :
	oldTime	( 0 )
{
	reset();
}

void MacOSXTimer::reset( void )
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
	
	start = tv.tv_sec;
	
	oldTime = 0;
}
	
float MacOSXTimer::getDeltaTime( void ) const
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
	
	unsigned long curTime = ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
	unsigned long delta = curTime - oldTime;
	
	oldTime = curTime;
	
	//NSLog( @"delta=%i", delta );
	
	return delta;
}

unsigned long MacOSXTimer::getMilliseconds( void )
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
			
	return ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
}

unsigned long MacOSXTimer::getMicroseconds( void )
{
	return getMilliseconds() * 1000;
}

unsigned long MacOSXTimer::getMillisecondsCPU( void )
{
	return 0;
}

unsigned long MacOSXTimer::getMicrosecondsCPU( void )
{
	return 0;
}

MacOSXApplication::MacOSXApplication( void ) :
	resolution	( 1024, 768 ),
	//resolution	( 480, 320 ),
	application	( 0 )
    //cursor      ( nil )
{
}

MacOSXApplication::~MacOSXApplication( void )
{
	delete application;
}
	
const bool MacOSXApplication::Init( void )
{
	char resDirectory[ 1024 ];
	char docDirectory[ 1024 ];
	
	[ [ [ NSBundle mainBundle] resourcePath ] getCString : resDirectory
											   maxLength : sizeof( resDirectory ) - 1
												encoding : NSASCIIStringEncoding ];
	
	[ [ NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
													 getCString : docDirectory
													 maxLength : sizeof( docDirectory ) - 1
													 encoding : NSASCIIStringEncoding ];
    
    mkdir( ( docDirectory + String( "/Menge/" ) ).c_str(), 0777 );
	
	application = new Application( this, &logSystem, resDirectory + String( "/" ), docDirectory + String( "/Menge/" ), "MACOSX" );
	
	Application::keep( application );
	
	application->setDesktopResolution( Resolution( [ [ NSScreen mainScreen ] frame ].size.width, [ [ NSScreen mainScreen ] frame ].size.height ) );
	application->setMaxClientAreaSize( resolution.getWidth(), resolution.getHeight() );
	
	if( !application->initialize( "application", "" ) )
		return false;
	
	ConstString lp( "rus" );
	application->setLanguagePack( lp );
	
	if( !application->loadGame( true ) )
		return false;
	
	if( !application->createRenderWindow( 0, 0 ) )
		return false;
	
	if( !application->initGame( "" ) )
		return false;
    
    showMessageBox( "Hello!", "God", 0 );
	
	return true;
}

void MacOSXApplication::Frame( void )
{
    [ pMengineOpenGLView updateCursor ];
    
    application->onRender();
    application->onUpdate();
    application->onTick( timer.getDeltaTime() );
    application->onFlush();
}

/*void MacOSXApplication::FocusCursor( bool focus )
{
    if( focus )
    {
        [ cursor set ];
    }
    else
    {
        [ [ NSCursor arrowCursor ] set ];
    }
}*/

void MacOSXApplication::stop( void )
{
    exit( 0 );
}

const Resolution & MacOSXApplication::getDesktopResolution( void ) const
{
	return resolution;
}

void MacOSXApplication::minimizeWindow( void )
{
}

void MacOSXApplication::setHandleMouse( bool _handle )
{
}

void MacOSXApplication::setCursorPosition( int _x, int _y )
{
}

void MacOSXApplication::showMessageBox( const String & _message, const String & _header, unsigned int _style )
{
	/*NSString * title = [ [ NSString alloc ] initWithFormat : @"%s", _header.c_str() ];
	NSString * message = [ [ NSString alloc ] initWithFormat : @"%s", _message.c_str() ];
	
	UIAlertView * alert = [ [ UIAlertView alloc ] initWithTitle : title message : message delegate : nil cancelButtonTitle : @"OK" otherButtonTitles : nil ];
	[ alert show ];
	[ alert release ];
	
	[ title release ];
	[ message release ];*/
}

TimerInterface * MacOSXApplication::getTimer( void ) const
{
	return ( TimerInterface * )&timer;
}
	
void MacOSXApplication::showKeyboard( void )
{
	//[ pEAGLView showKeyboard ];
}

void MacOSXApplication::hideKeyboard( void )
{
	//[ pEAGLView hideKeyboard ];
}

void MacOSXApplication::ansiToUtf8( const String & _ansi, String & _utf8 )
{
	NSString * ansi = [ [ NSString alloc ] initWithCString : _ansi.c_str() encoding : NSWindowsCP1251StringEncoding ];
	
	_utf8 = [ ansi cStringUsingEncoding : NSUTF8StringEncoding ];
	
	[ ansi release ];
}

void MacOSXApplication::utf8ToAnsi( const String & _utf8, String & _ansi )
{
	NSString * utf8 = [ [ NSString alloc ] initWithCString : _ansi.c_str() encoding : NSUTF8StringEncoding ];
	
	_ansi = [ utf8 cStringUsingEncoding : NSWindowsCP1251StringEncoding ];
	
	[ utf8 release ];
}
	
/*String MacOSXApplication::ansiToUtf8( const String& _ansi )
{
	OSStatus err = noErr;
	TECObjectRef tec = 0;
	ByteCount bytesConsumed = 0;
	ByteCount bytesProduced = 0;
	
	TextEncoding inputEncoding	= CreateTextEncoding( CFStringGetSystemEncoding(),
													 kTextEncodingDefaultVariant,
													 kTextEncodingDefaultFormat);
	
	TextEncoding outputEncoding = CreateTextEncoding( kTextEncodingUnicodeDefault,
													 kTextEncodingDefaultVariant,
													 kUnicodeUTF8Format);
	
	err = TECCreateConverter( &tec, inputEncoding, outputEncoding );
	
	std::size_t bufLen = _ansi.length() * 4;
	String out;
	char* buffer = new char[bufLen];
	if (err == noErr)
	{
		err = TECConvertText(tec,
							 (ConstTextPtr) _ansi.c_str(),
							 _ansi.length(),				// inputBufferLength
							 &bytesConsumed,				// actualInputLength
							 (TextPtr) buffer,			// outputBuffer
							 bufLen,					// outputBufferLength
							 &bytesProduced);			// actualOutputLength
		
		out.assign( buffer, bytesProduced );
		
		TECFlushText( tec, (TextPtr) buffer, bufLen, &bytesProduced );
		
		if( bytesProduced > 0 )
		{
			out += String( buffer, bytesProduced );
		}
		TECDisposeConverter(tec);
	}
	delete[] buffer;
	return out;
}
//////////////////////////////////////////////////////////////////////////
String MacOSXApplication::utf8ToAnsi( const String& _utf8 )
{
	OSStatus err = noErr;
	TECObjectRef tec = 0;
	ByteCount bytesConsumed = 0;
	ByteCount bytesProduced = 0;
	
	TextEncoding outputEncoding	= CreateTextEncoding( CFStringGetSystemEncoding(),
													 kTextEncodingDefaultVariant,
													 kTextEncodingDefaultFormat);
	
	TextEncoding inputEncoding = CreateTextEncoding( kTextEncodingUnicodeDefault,
													kTextEncodingDefaultVariant,
													kUnicodeUTF8Format);
	
	err = TECCreateConverter( &tec, inputEncoding, outputEncoding );
	
	std::size_t bufLen = _utf8.length();
	String out;
	char* buffer = new char[bufLen];
	if (err == noErr)
	{
		err = TECConvertText(tec,
							 (ConstTextPtr) _utf8.c_str(),
							 _utf8.length(),				// inputBufferLength
							 &bytesConsumed,				// actualInputLength
							 (TextPtr) buffer,			// outputBuffer
							 bufLen,					// outputBufferLength
							 &bytesProduced);			// actualOutputLength
		
		out.assign( buffer, bytesProduced );
		
		TECFlushText( tec, (TextPtr) buffer, bufLen, &bytesProduced );
		if( bytesProduced > 0 )
		{
			out += String( buffer, bytesProduced );
		}
		TECDisposeConverter(tec);
	}
	
	delete[] buffer;
	return out;
}*/

void MacOSXApplication::utf8Count( const String& _utf8, std::size_t & _size )
{

}

DynamicLibraryInterface * MacOSXApplication::loadDynamicLibrary( const String & _filename )
{
	return 0;
}

void MacOSXApplication::unloadDynamicLibrary( DynamicLibraryInterface * _lib )
{
}

void MacOSXApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
{
    //[ [ NSApplication sharedApplication ].delegate setFullscreen : _fullscreen ];
    needFullscreen = _fullscreen;
}

void MacOSXApplication::notifyVsyncChanged( bool _vsync )
{
}

void MacOSXApplication::notifyCursorModeChanged( bool _mode )
{
}

void MacOSXApplication::notifyCursorIconSetup( const String & _filename )
{
#if 0
    NSImage * image = [ [ NSImage alloc ] initWithContentsOfFile : /*@"/Users/Dgut/Library/Developer/Xcode/DerivedData/Mengine-fqhelhrkygupcphcmejdnlwxmaiv/Build/Products/Debug/Mengine.app/Contents/Resources/data/Resources/Cursors/wagtail.ani"/*/ [ NSString stringWithCString : _filename.c_str() encoding:NSASCIIStringEncoding ] ];
    
    [ cursor release ];
    cursor = [ [ NSCursor alloc ] initWithImage : image hotSpot : NSMakePoint( 0, 0 ) ];
    [ cursor set ];
    
    [ image release ];
#endif
    FILE * stream = fopen( _filename.c_str(), "rb" );
    
    short x, y;
    fseek( stream, 10, SEEK_SET );
    fread( &x, 2, 1, stream );
    fseek( stream, 12, SEEK_SET );
    fread( &y, 2, 1, stream );
    
    fclose( stream );
    
    NSImage * image = [ [ NSImage alloc ] initWithContentsOfFile : /*@"/Users/Dgut/Library/Developer/Xcode/DerivedData/Mengine-fqhelhrkygupcphcmejdnlwxmaiv/Build/Products/Debug/Mengine.app/Contents/Resources/data/Resources/Cursors/wagtail.ani"/*/ [ NSString stringWithCString : _filename.c_str() encoding:NSASCIIStringEncoding ] ];
    
    NSCursor * cursor = [ [ NSCursor alloc ] initWithImage : image hotSpot : NSMakePoint( x, y ) ];
    pMengineOpenGLView.cursor = cursor;
    [ pMengineOpenGLView resetCursorRects ];
    
    [ cursor release ];
    [ image release ];
}

void MacOSXApplication::notifyCursorClipping( const Viewport & _viewport )
{
}

void MacOSXApplication::notifyCursorUnClipping( void )
{
}

void MacOSXApplication::setAsScreensaver( bool _set )
{
}
	
}
