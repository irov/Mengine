/*
 *  iOSApplication.cpp
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "iOSApplication.h"
#include "Menge/Application.h"
#include "EAGLView.h"
#include <sys/time.h>

namespace Menge
{
	
void iOSLogger::log( const void * _data, int _count, EMessageLevel _level )
{
}

void iOSLogger::flush( void )
{
}

void iOSLogSystem::setVerboseLevel( EMessageLevel _level )
{
}

void iOSLogSystem::logMessage( const String & _message, EMessageLevel _level )
{
	NSLog( @"%s", _message.c_str() );
}
	
bool iOSLogSystem::registerLogger( LoggerInterface * _logger )
{
	return true;
}
	
void iOSLogSystem::unregisterLogger( LoggerInterface * _logger )
{
}
	
iOSTimer::iOSTimer( void ) :
	oldTime	( 0 )
{
	reset();
}

void iOSTimer::reset( void )
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
	
	start = tv.tv_sec;
	
	oldTime = 0;
}
	
float iOSTimer::getDeltaTime( void ) const
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
	
	unsigned long curTime = ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
	unsigned long delta = curTime - oldTime;
	
	oldTime = curTime;
	
	return delta;
}

unsigned long iOSTimer::getMilliseconds( void )
{
	timeval tv;
	
	gettimeofday( &tv, 0 );
			
	return ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
}

unsigned long iOSTimer::getMicroseconds( void )
{
	return getMilliseconds() * 1000;
}

unsigned long iOSTimer::getMillisecondsCPU( void )
{
	return 0;
}

unsigned long iOSTimer::getMicrosecondsCPU( void )
{
	return 0;
}

iOSApplication::iOSApplication( void ) :
	resolution	( 1024, 768 ),
	//resolution	( 480, 320 ),
	application	( 0 )
{
}
	
iOSApplication::~iOSApplication( void )
{
	delete application;
}
	
const bool iOSApplication::Init( void )
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
	
	application = new Application( this, &logSystem, resDirectory + String( "/" ), docDirectory + String( "/" ), "IPAD" );
	
	Application::keep( application );
	
	application->setDesktopResolution( resolution );
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
	
	return true;
}

void iOSApplication::Frame( void )
{
	application->onRender();
	application->onUpdate();
	application->onTick( timer.getDeltaTime() );
	application->onFlush();
}
	
void iOSApplication::stop( void )
{
}

const Resolution & iOSApplication::getDesktopResolution( void ) const
{
	return resolution;
}

void iOSApplication::minimizeWindow( void )
{
}

void iOSApplication::setHandleMouse( bool _handle )
{
}

void iOSApplication::setCursorPosition( int _x, int _y )
{
}

void iOSApplication::showMessageBox( const String & _message, const String & _header, unsigned int _style )
{
	NSString * title = [ [ NSString alloc ] initWithFormat : @"%s", _header.c_str() ];
	NSString * message = [ [ NSString alloc ] initWithFormat : @"%s", _message.c_str() ];
	
	UIAlertView * alert = [ [ UIAlertView alloc ] initWithTitle : title message : message delegate : nil cancelButtonTitle : @"OK" otherButtonTitles : nil ];
	[ alert show ];
	[ alert release ];
	
	[ title release ];
	[ message release ];
}

TimerInterface * iOSApplication::getTimer( void ) const
{
	return ( TimerInterface * )&timer;
}
	
void iOSApplication::showKeyboard( void )
{
	[ pEAGLView showKeyboard ];
}

void iOSApplication::hideKeyboard( void )
{
	[ pEAGLView hideKeyboard ];
}

void iOSApplication::ansiToUtf8( const String & _ansi, String & _utf8 )
{
    _utf8 = _ansi;    
//	NSString * ansi = [ [ NSString alloc ] initWithCString : _ansi.c_str() encoding : NSWindowsCP1251StringEncoding ];
//	_utf8 = [ ansi cStringUsingEncoding : NSUTF8StringEncoding ];
//	[ ansi release ];
}

void iOSApplication::utf8ToAnsi( const String & _utf8, String & _ansi )
{
    _ansi = _utf8;
//	NSString * utf8 = [ [ NSString alloc ] initWithCString : _ansi.c_str() encoding : NSUTF8StringEncoding ];
//	_ansi = [ utf8 cStringUsingEncoding : NSWindowsCP1251StringEncoding ];	
//	[ utf8 release ];
}
    
void iOSApplication::utf8Count( const String& _utf8, std::size_t & _size )
{
        
}
	
/*String iOSApplication::ansiToUtf8( const String& _ansi )
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
String iOSApplication::utf8ToAnsi( const String& _utf8 )
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

DynamicLibraryInterface * iOSApplication::loadDynamicLibrary( const String & _filename )
{
	return 0;
}

void iOSApplication::unloadDynamicLibrary( DynamicLibraryInterface * _lib )
{
}

void iOSApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
{
}

void iOSApplication::notifyVsyncChanged( bool _vsync )
{
}

void iOSApplication::notifyCursorModeChanged( bool _mode )
{
}

void iOSApplication::notifyCursorIconSetup( const String & _filename )
{
}

void iOSApplication::notifyCursorClipping( const Viewport & _viewport )
{
}

void iOSApplication::notifyCursorUnClipping( void )
{
}

void iOSApplication::setAsScreensaver( bool _set )
{
}
	
}
