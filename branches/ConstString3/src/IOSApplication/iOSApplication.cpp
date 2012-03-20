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

#include <AudioToolbox/AudioToolbox.h>

#include "Interface/FileSystemInterface.h"

extern EAGLView * pEAGLView;

namespace Menge
{
    iOSFileLogger::iOSFileLogger()
        : m_verboseLevel(LM_INFO)
    {
        
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileLogger::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileLogger::validVerboseLevel( EMessageLevel _level ) const
    {
        if( m_verboseLevel < _level )
        {
            return false;
        }
        
        return true;
    }
    
void iOSFileLogger::log( const char * _data, int _count, EMessageLevel _level )
{
    NSLog( @"%s", _data );
}

void iOSFileLogger::flush( void )
{
}
	
iOSTimer::iOSTimer( void ) 
    : oldTime( 0 )
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
	releaseInterfaceSystem( application );
    application = NULL;
}
	
const bool iOSApplication::Init( void )
{
		
    AudioSessionInitialize(NULL, NULL, NULL, NULL);
    
    
    char resDirectory[ 1024 ];
    
	[ [ [ NSBundle mainBundle] resourcePath ] getCString : resDirectory
											   maxLength : sizeof( resDirectory ) - 1
												encoding : NSASCIIStringEncoding ];    
    
    m_applicationPath = resDirectory + String("/");
    
    NSLog( @"m_applicationPath %s"
          , m_applicationPath.c_str() 
          );      
    
    char docDirectory[ 1024 ];
    
	[ [ NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
     getCString : docDirectory
     maxLength : sizeof( docDirectory ) - 1
     encoding : NSASCIIStringEncoding ];   
    
    m_userPath = docDirectory + String("/");
    
    NSLog( @"m_userPath %s"
          , m_userPath.c_str() 
          );  
    
    m_loggerConsole = new iOSFileLogger();
    
    //NSLog( @"resDirectory %s", resDirectory );
    //NSLog( @"docDirectory %s", docDirectory );
    
    if( initInterfaceSystem( &application ) == false )
    {
        return false;
    }
    
	//application = new Application( this, &logSystem, resDirectoryFull, docDirectoryFull, "IPAD" );
	
	//Application::keep( application );
    
    String platformName("IPAD");
    String commandLine; //Empty
    
    if( application->initialize( this, platformName, commandLine ) == false )
    {
        //LOGGER_ERROR(m_logSystem)( "Application initialize failed" 
        // );
            
        return false;
    }    
	
	application->setDesktopResolution( resolution );
	application->setMaxClientAreaSize( resolution.getWidth(), resolution.getHeight() );
	
    ServiceProviderInterface * serviceProvider = application->getServiceProvider();
    
    String configFile = "application";
    application->loadConfig( configFile );
    
    ConstString c_languagePack("rus");
    application->setLanguagePack( c_languagePack );
    
    String baseDir = m_applicationPath + "data/";
    
    application->setBaseDir(baseDir);
    
    if( application->loadGame() == false )
    {
        NSLog( @"application loadGame failed!"
              );
        
        return false;
    }
    
    if( application->loadPersonality() == false )
    {
        NSLog( @"application loadPersonality failed!"
              );
        
        return false;
    }    
    
	//if( !application->initialize( "application", "" ) )
	//	return false;
	
	//ConstString lp( "rus" );
	//application->setLanguagePack( lp );
	
	//if( !application->loadGame( true ) )
	//	return false;
	
	if( application->createRenderWindow( 0, 0 ) == false )
    {
        NSLog( @"application createRenderWindow failed!"
              );
        
		return false;
    }
	
    String scriptInit;
    
	if( application->initGame( scriptInit ) == false )
    {        
        NSLog( @"application initGame failed!"
              );
        
		return false;
    }
	
	return true;
}

void iOSApplication::Frame( void )
{
    if( application == 0 )
    {
        return;
    }
    
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
    
    void iOSApplication::Finalize( void )
    {
        if( application )
        {
            application->finalize();
        
            ServiceProviderInterface * serviceProvider = application->getServiceProvider();
            
            logService = serviceProvider->getServiceT<LogServiceInterface>("LogService");
        
            logService->unregisterLogger( m_loggerConsole );
            delete m_loggerConsole;
        }        
        
        delete application;
    }
    
    bool iOSApplication::OpenAL_OtherAudioIsPlaying()
    {
        int playing;
        UInt32 size = sizeof(playing);
        AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &size, &playing);
        
        return (bool)playing;
    }
    
    void iOSApplication::TurnSoundOn()
    {
        application->onTurnSound( true );
    }

    void iOSApplication::TurnSoundOff()
    {        
        application->onTurnSound( false );        
    }
    
ApplicationInterface * iOSApplication::getApplication() const
{
    return application;
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

    
    
    //void iOSApplication::notifySoundInitialize()
    //{
    //    AudioSessionSetActive(true);
    //}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class iOSApplicationFileService
         : public ServiceListenerInterface
        {
        public:
            iOSApplicationFileService( iOSApplication * _application )
            : m_application(_application)
            {
            }
            
        protected:
            void onRegistryService( ServiceProviderInterface * _serviceProvide, ServiceInterface * _service )
            {
                m_application->setupFileService();
            }
            
        protected:
            iOSApplication * m_application;
        };
        //////////////////////////////////////////////////////////////////////////
        class iOSApplicationLogService
         : public ServiceListenerInterface
        {
        public:
            iOSApplicationLogService( iOSApplication * _application )
            : m_application(_application)
            {
            }
            
        protected:
            void onRegistryService( ServiceProviderInterface * _serviceProvide, ServiceInterface * _service )
            {
                m_application->setupLogService();
            }
            
        protected:
            iOSApplication * m_application;
        };
    }    
    
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
        
        m_serviceProvider->addServiceListener( "LogService", new iOSApplicationLogService(this) );
        m_serviceProvider->addServiceListener( "FileService", new iOSApplicationFileService(this) );
    }
    
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setupLogService()
    {
        m_logService = m_serviceProvider->getServiceT<LogServiceInterface>("LogService");
        
        if( m_loggerConsole != NULL )
        {
            m_logService->registerLogger( m_loggerConsole );
        }
        
# ifndef MENGE_MASTER_RELEASE
        m_logService->setVerboseLevel( LM_LOG );
# endif
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setupFileService()
    {
        FileServiceInterface * fileService = m_serviceProvider->getServiceT<FileServiceInterface>("FileService");
        
        NSLog( @"iOSApplication::setupFileService m_applicationPath %s"
              , m_applicationPath.c_str()
              );
        
        // mount root  
        if( fileService->mountFileSystem( ConstString(""), m_applicationPath, ConstString("dir"), false ) == false )
        {
            LOGGER_ERROR(m_logService)( "iOSApplication: failed to mount application directory %s"
                                       , m_applicationPath.c_str()
                                       ); 
        }
        
        NSLog( @"iOSApplication::setupFileService m_userPath %s"
              , m_userPath.c_str()
              );
        
        // mount user directory
        if( fileService->mountFileSystem( ConstString("user"), m_userPath, ConstString("dir"), true ) == false )
        {
            LOGGER_ERROR(m_logService)( "iOSApplication: failed to mount user directory %s"
                                       , m_userPath.c_str()
                                       );
        }
    }

void iOSApplication::setAsScreensaver( bool _set )
{
}
	
}
