/*
 *  MacOSXApplication.cpp
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MacOSXApplication.h"
#include "Application.h"
#include "FileEngine.h"
#include "MengineOpenGLView.h"
#include "AppDelegate.h"
#include <sys/stat.h>
#include <sys/time.h>

extern bool DONTHIDECURSOR111111;

extern int g_argc;
extern char * * g_argv;

namespace Menge
{

/*void MacOSXLogger::log( const void * _data, int _count, EMessageLevel _level )
{
}

void MacOSXLogger::flush( void )
{
}*/

void MacOSXLogSystem::setVerboseLevel( EMessageLevel _level )
{
}

void MacOSXLogSystem::logMessage( const String & _message, EMessageLevel _level )
{
	NSLog( @"%s", _message.c_str() );
}
	
bool MacOSXLogSystem::registerLogger( OutputStreamInterface * _logger )
{
	return true;
}
	
void MacOSXLogSystem::unregisterLogger( OutputStreamInterface * _logger )
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
	//resolution	( 1024, 768 ),
	resolution	( 480, 320 ),
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
	
	[ [ NSSearchPathForDirectoriesInDomains( /*NSDocumentDirectory*/NSLibraryDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
													 getCString : docDirectory
													 maxLength : sizeof( docDirectory ) - 1
													 encoding : NSASCIIStringEncoding ];
    
    String AlawarUserDir;
    
    AlawarUserDir += docDirectory;
    AlawarUserDir += "/Application Support/Alawar Entertainment/";
    
    
    mkdir( AlawarUserDir.c_str(), 0777 );
    
    String UserDir;
    
    UserDir += AlawarUserDir;    
    UserDir += "Sinbad/";
    
    mkdir( UserDir.c_str(), 0777 );
    /*mkdir( "%PUBLIC%/Library/Application Support/Alawar Entertainment", 0777 );
    int err = errno;
    mkdir( "Library/Application Support/Alawar Entertainment/Sinbad/", 0777 );*/
    //[[ NSFileManager defaultManager ] createDirectoryAtPath:@"/Library/Application Support/Alawar Entertainment" withIntermediateDirectories:YES attributes:nil error:nil];
	
	//application = new Application( this, &logSystem, resDirectory + String( "/" ), docDirectory + String( "/Menge/" ), "MACOSX" );
    
    String commandLine = " ";
    for( int i = 0; i < g_argc; i++ )
    {
        commandLine += g_argv[ i ];
        commandLine += " ";
    }
    
    String scriptInit;
    std::size_t fpos = commandLine.find( " -s:", 0 );
    if( fpos != String::npos )
    {
        String substring = "";
        if( commandLine[fpos+4] == '\"' )
        {
            std::size_t qpos = commandLine.find( '\"', fpos+5 );
            substring = commandLine.substr( fpos+5, qpos-(fpos+5) );
        }
        else
        {
            std::size_t spos = commandLine.find( ' ', fpos+4 );
            substring = commandLine.substr( fpos+4, spos-(fpos+4) );
        }
        scriptInit = substring;
    }
       
    application = new Application( this, UserDir, scriptInit, 0 );
	
	Application::keep( application );
    //NSRect frame = [ [ NSScreen mainScreen ] frame ];
    //NSRect visFrame = [ [ NSScreen mainScreen ] visibleFrame ];
    
    application->setLoggingLevel( LM_MAX );
	
	application->setDesktopResolution( Resolution( [ [ NSScreen mainScreen ] frame ].size.width, [ [ NSScreen mainScreen ] frame ].size.height ) );
	application->setMaxClientAreaSize([ [ NSScreen mainScreen ] visibleFrame ].size.width, [ [ NSScreen mainScreen ] visibleFrame ].size.height  );
    
    application->setWorkAreaSize( [ [ NSScreen mainScreen ] visibleFrame ].size.width, [ [ NSScreen mainScreen ] visibleFrame ].size.height  );
    
    application->setAllowAutoFullscreen(false);
    
    std::string defLang = s_getDefaultLanguage();
    std::string languagePack;
    
    if( defLang.empty() == false )
    {
        languagePack = defLang;
    }
    
    application->setLanguagePack( languagePack, "" );
    
    if( !application->initialize( "application.xml", commandLine, true ) )
		return false;
	
	/*if( !application->createRenderWindow( 0, 0 ) )
		return false;
	
	if( !application->initGame() )
		return false;*/
    
    showMessageBox( "Hello!", "God", 0 );
    
    needFullscreen = application->getFullscreenMode();
    
    //application->setFullscreenMode( !application->getFullscreenMode() );
    //application->setFullscreenMode( !application->getFullscreenMode() );
	
	return true;
}
    
    bool isAfter = false;
    
void MacOSXApplication::After( void )
{
    if( !application->createRenderWindow( 0, 0 ) )
    {
		return;
	}
    
	if( !application->initGame() )
		return;
    [ [pMengineOpenGLView window] center ];
    ////NSRect frame = NSMakeRect(100,1,800,600);
    //[ [pMengineOpenGLView window] setFrame :NSMakeRect(0,100,1024,768) display:YES ];
    isAfter = true;
}
    
std::string MacOSXApplication::s_getDefaultLanguage( void )
{
    NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    NSString* preferredLang = [languages objectAtIndex:0];
    
    std::string lang = [ preferredLang cStringUsingEncoding:NSASCIIStringEncoding ];
    
    if( lang == "en" )
        lang += 'g';
    else if( lang == "ru" )
        lang += 's';
    else if( lang == "de" )
        lang += 'u';
    else if( lang == "fr" )
        lang += 'a';
    else if( lang == "es" )
        lang = "spa";
    else if( lang == "ko" )
        lang += 'r';
    else if( lang == "hu" )
        lang += 'n';
    else if( lang == "mn" )
        lang = "mon";
    else if( lang == "pl" )
        lang = "pol";
    else if( lang == "ja" )
        lang = "jpn";
    else
        lang = "eng";
    
    return lang;
    
/*
 <LanguagePack Name = "eng" Path = "eng" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "rus" Path = "rus" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "deu" Path = "deu" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "fra" Path = "fr" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "spa" Path = "es" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "kor" Path = "kor" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "hun" Path = "hun" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "mon" Path = "mon" Description = "Resource.xml" NoPak = "1"/>
 <LanguagePack Name = "pol" Path = "pol" Description = "Resource.xml" NoPak = "1"/>
 */
    
    /*std::string defLanguage;
    CFArrayRef localizationList = NULL;
    CFBundleRef bundle = CFBundleGetMainBundle();
    if( bundle != NULL )
    {
        CFArrayRef bundleLocList = CFBundleCopyBundleLocalizations( bundle );
        if( bundleLocList != NULL )
        {
            localizationList = CFBundleCopyPreferredLocalizationsFromArray( bundleLocList );
            CFRelease( bundleLocList );
        }
    }
    
    if( localizationList != NULL )
    {
        if( CFGetTypeID( localizationList ) == CFArrayGetTypeID()
           && CFArrayGetCount( localizationList ) > 0 )
        {
            CFStringRef languageName = static_cast<CFStringRef>( CFArrayGetValueAtIndex( localizationList, 0 ) );
            
            if ( languageName != NULL 
                && CFGetTypeID( languageName ) == CFStringGetTypeID() )
            {
                int stringBufLen = static_cast<int>( CFStringGetLength( languageName ) );
                std::vector<char> stringBuf;
                stringBuf.resize( stringBufLen + 1 );
                CFStringGetCString(  languageName, &stringBuf[0], stringBuf.size(), kCFStringEncodingASCII );
                defLanguage.assign( &stringBuf[0] );
            }
        }
    }
    
    std::transform( defLanguage.begin(), defLanguage.end(), 
                   defLanguage.begin(), std::ptr_fun( &tolower ) );
    
    return defLanguage;*/
}

void MacOSXApplication::Frame( void )
{
    if( !isAfter )
        return;
    
    [ pMengineOpenGLView updateCursor ];
    
    /*application->onRender();
    application->onUpdate();
    application->onTick( timer.getDeltaTime() );
    application->onFlush();*/
    bool rendered = false;
    if( application->isFocus() == true )
    {
        rendered = application->onRender();
    }
    
    float frameTime = timer.getDeltaTime();
    
    bool updating = application->onUpdate();
    
    if( updating == true )
    {
        application->onTick( frameTime );
    }
    else
    {
        //Sleep(100);
    }
    
    /*if( m_fpsMonitor )
    {
        m_fpsMonitor->monitor();
    }*/
    
    if( rendered )
    {
        application->onFlush();
    }
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
	return application->getDesktopResolution();
    //return resolution;
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

String MacOSXApplication::ansiToUtf8( const String & _ansi )
{
	NSString * ansi = [ [ NSString alloc ] initWithCString : _ansi.c_str() encoding : NSWindowsCP1251StringEncoding ];
	
	String _utf8 = [ ansi cStringUsingEncoding : NSUTF8StringEncoding ];
	
	[ ansi release ];
    
    return _utf8;
}

String MacOSXApplication::utf8ToAnsi( const String & _utf8 )
{
	NSString * utf8 = [ [ NSString alloc ] initWithCString : _utf8.c_str() encoding : NSUTF8StringEncoding ];
	
	String _ansi = [ utf8 cStringUsingEncoding : NSWindowsCP1251StringEncoding ];
	
	[ utf8 release ];
    
    return _ansi;
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

DynamicLibraryInterface * MacOSXApplication::load( const String & _filename )
{
	return 0;
}

void MacOSXApplication::unload( DynamicLibraryInterface * _lib )
{
}

void MacOSXApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
{
    //[ [ NSApplication sharedApplication ].delegate setFullscreen : _fullscreen ];
    //needFullscreen = _fullscreen;
}

void MacOSXApplication::notifyVsyncChanged( bool _vsync )
{
}

void MacOSXApplication::notifyCursorModeChanged( bool _mode )
{
    DONTHIDECURSOR111111 = _mode;
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
    
    /*NSImage * image = [ [ NSImage alloc ] initWithContentsOfFile : [ NSString stringWithCString : _filename.c_str() encoding:NSASCIIStringEncoding ] ];
    
    NSCursor * cursor = [ [ NSCursor alloc ] initWithImage : image hotSpot : NSMakePoint( x, y ) ];
    pMengineOpenGLView.cursor = cursor;
    
    [ cursor release ];
    [ image release ];*/
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

bool MacOSXApplication::openUrlInDefaultBrowser( const String & _url )
{
    CFURLRef url = CFURLCreateWithBytes (
                                         NULL,                        // allocator
                                         (UInt8*)_url.c_str(),     // URLBytes
                                         _url.length(),            // length
                                         kCFStringEncodingASCII,      // encoding
                                         NULL                         // baseURL
                                         );
    LSOpenCFURLRef(url,0);
    CFRelease(url);
    
    return true;
}

bool MacOSXApplication::executeProgram( const String & _programPath )
{
    system( _programPath.c_str() );
    
    return true;
}
	
}
