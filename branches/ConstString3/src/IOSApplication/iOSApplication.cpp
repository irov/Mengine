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
#include "Interface/UnicodeInterface.h"


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
    
const WString & iOSApplication::getCurrentPath() const
    {
        static WString empty;
        return empty;
        
    }
	
const bool iOSApplication::Init( void )
{
		
    AudioSessionInitialize(NULL, NULL, NULL, NULL);
    
    NSLog(@"wchar_t %d", sizeof(wchar_t));
    
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
    
    ServiceProviderInterface * serviceProvider = application->getServiceProvider();
    
    
    UnicodeInterface * unicodeInterface = serviceProvider->getServiceT<UnicodeInterface>("Unicode");
        	
	application->setDesktopResolution( resolution );
	application->setMaxClientAreaSize( resolution.getWidth(), resolution.getHeight() );
	
    ConstString c_languagePack("rus");
    application->setLanguagePack( c_languagePack );
    
    WString config_file = L"application";
    WString settings_file = L"settings.ini";
    if( application->loadConfig( config_file, settings_file ) == false )
    {
            return false;
    }
    
    WString baseDir = m_applicationPath + L"data/";
    
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

void iOSApplication::showMessageBox( const WString & _message, const WString & _header, unsigned int _style )
{
	NSString * title = [ [ NSString alloc ] initWithFormat : @"%S", _header.c_str() ];
	NSString * message = [ [ NSString alloc ] initWithFormat : @"%S", _message.c_str() ];
	
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

DynamicLibraryInterface * iOSApplication::loadDynamicLibrary( const WString & _filename )
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

void iOSApplication::notifyCursorIconSetup( const WString & _filename )
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
        //m_logService->setVerboseLevel( LM_INFO );
# ifndef MENGE_MASTER_RELEASE
        m_logService->setVerboseLevel( LM_LOG );
# endif
    }
    ////
    class Utf8
    {
    public:
        Utf8(const wchar_t* wsz): m_utf8(NULL)
        {
            // OS X uses 32-bit wchar
            const int bytes = wcslen(wsz) * sizeof(wchar_t);
            
            CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
                                                            (const UInt8*)wsz, bytes, 
                                                            kCFStringEncodingUTF32LE, false, 
                                                            kCFAllocatorNull
                                                            );
            
            const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
            m_utf8 = new char[bytesUtf8];
            CFStringGetFileSystemRepresentation(str, m_utf8, bytesUtf8);
            CFRelease(str);
        }   
        
        ~Utf8() 
        { 
            if( m_utf8 )
            {
                delete[] m_utf8;
            }
        }
        
    public:
        const char* c_str() const { return m_utf8; }
        
    private:
        char* m_utf8;
    };
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setupFileService()
    {
        UnicodeInterface * unicodeInterface = m_serviceProvider->getServiceT<UnicodeInterface>("Unicode");
        
        char resDirectory[ 1024 ];
        
        [ [ [ NSBundle mainBundle] resourcePath ] getCString : resDirectory
                                                   maxLength : sizeof( resDirectory ) - 1
                                                    encoding : NSUTF8StringEncoding ];    
        
        String str_resDirectory(resDirectory);
        
        size_t size_str_resDirectory = str_resDirectory.size();
        
        bool wstr_resDirectory_successful;
        WString wstr_resDirectory = unicodeInterface->utf8ToUnicode(str_resDirectory, wstr_resDirectory_successful);
        
        
        size_t size_wstr_resDirectory = wstr_resDirectory.size();
        
        m_applicationPath = wstr_resDirectory + MENGE_FOLDER_DELIM;
        
        const Utf8 utf8_applicationPath(m_applicationPath.c_str());
        
        NSLog( @"iOSApplication::setupFileService m_applicationPath '%ls'"
              , m_applicationPath.c_str()
              );
        
        printf("iOSApplication::setupFileService m_applicationPath %ls"
               , m_applicationPath.c_str()
               );
        
        LOGGER_ERROR(m_logService)( "iOSApplication::setupFileService m_applicationPath %ls"
                                   , m_applicationPath.c_str()
                                   ); 
        
        NSLog( @"iOSApplication::setupFileService m_applicationPath '%s'"
              , utf8_applicationPath.c_str()
              );

        FileServiceInterface * fileService = m_serviceProvider->getServiceT<FileServiceInterface>("FileService");
        
        // mount root  
        if( fileService->mountFileSystem( ConstString(""), m_applicationPath, ConstString("dir"), false ) == false )
        {
            LOGGER_ERROR(m_logService)( "iOSApplication: failed to mount application directory %S"
                                       , m_applicationPath.c_str()
                                       ); 
        }
        
        char docDirectory[ 1024 ];
        
        [ [ NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
         getCString : docDirectory
         maxLength : sizeof( docDirectory ) - 1
         encoding : NSUTF8StringEncoding ];
        
        String str_docDirectory(docDirectory);
        
        bool wstr_docDirectory_successful;
        WString wstr_docDirectory = unicodeInterface->utf8ToUnicode(str_docDirectory, wstr_docDirectory_successful);
        
        m_userPath = wstr_docDirectory + MENGE_FOLDER_DELIM;
        
        const Utf8 utf8_userPath(m_userPath.c_str());
        
        NSLog( @"iOSApplication::setupFileService m_userPath '%s'"
              , utf8_userPath.c_str()
              );
        
        // mount user directory
        if( fileService->mountFileSystem( ConstString("user"), m_userPath, ConstString("dir"), true ) == false )
        {
            LOGGER_ERROR(m_logService)( "iOSApplication: failed to mount user directory %S"
                                       , m_userPath.c_str()
                                       );
        }
    }

void iOSApplication::setAsScreensaver( bool _set )
{
}
	
}
