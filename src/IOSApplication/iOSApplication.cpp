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

#include "StartupConfigLoader/StartupConfigLoader.h"


extern EAGLView * pEAGLView;

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);
SERVICE_EXTERN(Application, Menge::ApplicationInterface);
SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);

SERVICE_EXTERN(RenderSystem, Menge::RenderSystemInterface);
SERVICE_EXTERN(RenderService, Menge::RenderServiceInterface);

SERVICE_EXTERN(PhysicSystem2D, Menge::PhysicSystem2DInterface);
SERVICE_EXTERN(PhysicService2D, Menge::PhysicService2DInterface);

SERVICE_EXTERN(UnicodeSystem, Menge::UnicodeSystemInterface);
SERVICE_EXTERN(UnicodeService, Menge::UnicodeServiceInterface);

SERVICE_EXTERN(FileSystem, Menge::FileSystemInterface);
SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

SERVICE_EXTERN(NotificationService, Menge::NotificationServiceInterface);
SERVICE_EXTERN(ScriptService, Menge::ScriptServiceInterface);

SERVICE_EXTERN(SoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SilentSoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SoundService, Menge::SoundServiceInterface);

SERVICE_EXTERN(AmplifierService, Menge::AmplifierServiceInterface);
SERVICE_EXTERN(InputService, Menge::InputServiceInterface);
SERVICE_EXTERN(ConverterService, Menge::ConverterServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);

extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
    extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
}

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////////
    void iOSFileLogger::log( const char * _data, int _count, EMessageLevel _level )
    {
        NSLog( @"%s", _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileLogger::flush( void )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSTimer::iOSTimer( void ) 
        : oldTime( 0 )
    {
        reset();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSTimer::reset( void )
    {
        timeval tv;

        gettimeofday( &tv, 0 );

        start = tv.tv_sec;

        oldTime = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    float iOSTimer::getDeltaTime( void ) const
    {
        timeval tv;

        gettimeofday( &tv, 0 );

        unsigned long curTime = ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
        unsigned long delta = curTime - oldTime;

        oldTime = curTime;

        return delta;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long iOSTimer::getMilliseconds( void )
    {
        timeval tv;

        gettimeofday( &tv, 0 );

        return ( tv.tv_sec - start ) * 1000 + tv.tv_usec / 1000;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long iOSTimer::getMicroseconds( void )
    {
        return getMilliseconds() * 1000;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long iOSTimer::getMillisecondsCPU( void )
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long iOSTimer::getMicrosecondsCPU( void )
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    iOSApplication::iOSApplication( void ) 
        : m_resolution(1024.f, 768.f)
        , m_application(NULL)
        , m_pluginMengeImageCodec(NULL)
        , m_pluginMengeSoundCodec(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSApplication::~iOSApplication( void )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * iOSApplication::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & iOSApplication::getCurrentPath() const
    {
        static String currentPath;
        return currentPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeApplicationService_()
    {   
        ApplicationInterface * application;
        if( createApplication( &application ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, application ) == false )
        {
            return false;
        }

        m_application = application;

        if( m_application->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application initialize failed" 
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeNotificationService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Notification Service..." );

        NotificationServiceInterface * notificationService;
        if( createNotificationService( &notificationService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, notificationService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeThreadEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Thread Service..." );

        ThreadSystemInterface * threadSystem;
        if( createThreadSystem( &threadSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, threadSystem) == false )
        {
            return false;
        }

        threadSystem->initialize();

        ThreadServiceInterface * threadService;
        if( createThreadService( &threadService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadService"
                );

            return false;               
        }

        if( SERVICE_REGISTRY( m_serviceProvider, threadService ) == false )
        {
            return false;
        }

        m_threadService = threadService;

        if( m_threadService->initialize( 2 ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeFileEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing File Service..." );

        FileSystemInterface * fileSystem;
        if( createFileSystem( &fileSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileSystem ) == false )
        {
            return false;
        }

        FileServiceInterface * fileService;
        if( createFileService( &fileService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileService ) == false )
        {
            return false;
        }

        m_fileService = fileService;

        if( m_fileService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to initialize fileService"
                );

            return false;
        }

        char resDirectory[ 1024 ];

        [ [ [ NSBundle mainBundle] resourcePath ] getCString : resDirectory
maxLength : sizeof( resDirectory ) - 1
encoding : NSUTF8StringEncoding ];    

        String str_resDirectory(resDirectory);

        LOGGER_ERROR(m_serviceProvider)( "iOSApplication::setupFileService wstr_resDirectory '%s'"
            , str_resDirectory.c_str()
            );

                
        m_currentPath = str_resDirectory + MENGE_FOLDER_DELIM + "data" + MENGE_FOLDER_DELIM;

        LOGGER_ERROR(m_serviceProvider)( "iOSApplication::setupFileService m_applicationPath %s"
            , m_currentPath.c_str()
            ); 

        // mount root  
        if( m_fileService->mountFileSystem( Helper::stringizeString(m_serviceProvider, ""), Helper::stringizeString(m_serviceProvider, m_currentPath), Helper::stringizeString(m_serviceProvider, "dir"), false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "iOSApplication: failed to mount application directory %s"
                , m_currentPath.c_str()
                ); 

            return false;
        }

        if( this->setupApplicationSetting_( m_fileService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed to setup application %s"
                , m_currentPath.c_str()
                );

            return false;
        }

        char docDirectory[1024];

        [ [ NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
getCString : docDirectory
maxLength : sizeof( docDirectory ) - 1
encoding : NSUTF8StringEncoding ];

        String str_docDirectory(docDirectory);

        m_userPath = str_docDirectory + MENGE_FOLDER_DELIM;

        LOGGER_ERROR(m_serviceProvider)( "iOSApplication::setupFileService m_userPath '%s'"
            , m_userPath.c_str()
            );

        // mount user directory
        if( m_fileService->mountFileSystem( Helper::stringizeString(m_serviceProvider, "user"), Helper::stringizeString(m_serviceProvider, m_userPath), Helper::stringizeString(m_serviceProvider, "dir"), true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "iOSApplication: failed to mount user directory %s"
                , m_userPath.c_str()
                );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeStringizeService_()
    {
        StringizeServiceInterface * stringizeService;
        if( createStringizeService( &stringizeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, stringizeService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeLogEngine_()
    {
        LogServiceInterface * logService;
        if( createLogService( &logService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, logService ) == false )
        {
            return false;
        }

        m_logService = logService;

        m_loggerConsole = new iOSFileLogger();

        m_logService->registerLogger( m_loggerConsole );

        EMessageLevel m_logLevel;

        m_logLevel = LM_ERROR;

        m_logService->setVerboseLevel( m_logLevel );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeUnicodeEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Unicode Service..." );

        UnicodeSystemInterface * unicodeSystem;
        if( createUnicodeSystem( &unicodeSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeSystem ) == false )
        {
            return false;
        }

        UnicodeServiceInterface * unicodeService;
        if( createUnicodeService( &unicodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeService ) == false )
        {
            return false;
        }

        m_unicodeService = unicodeService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeParticleEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );

        ParticleSystemInterface * particleSystem;
        if( createParticleSystem( &particleSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine_ Failed to initialize ParticleSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
        {
            return false;
        }

        ParticleServiceInterface * particleService;
        if( createParticleService( &particleService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine_ Failed to initialize ParticleService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, particleService ) == false )
        {
            return false;
        }

        m_particleService = particleService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializePhysicEngine2D_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Physics2D Service..." );

        PhysicSystem2DInterface * physicSystem2D;
        if( createPhysicSystem2D( &physicSystem2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize PhysicSystem2D"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, physicSystem2D ) == false )
        {
            return false;
        }

        if( physicSystem2D->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize Physics System 2D"
                );
        }

        PhysicService2DInterface * physicService2D;
        if( createPhysicService2D( &physicService2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize PhysicService2D"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, physicService2D ) == false )
        {
            return false;
        }

        m_physicService2D = physicService2D;

        if( m_physicService2D->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize Physics Service 2D"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeRenderEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Render Service..." );

        RenderSystemInterface * renderSystem;
        if( createRenderSystem( &renderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderSystem ) == false )
        {
            return false;
        }

        if( renderSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render System"
                );

            return false;
        }

        RenderServiceInterface * renderService;
        if( createRenderService( &renderService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderService ) == false )
        {
            return false;
        }

        m_renderService = renderService;

        if( m_renderService->initialize( 8000 ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( createSoundSystem( &soundSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound System"
                );

            return false;
        }

        SoundServiceInterface * soundService;
        if( createSoundService( &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound Engine"
                );

            m_serviceProvider->unregistryService( "SoundService" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeSilentSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Silent Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( createSilentSoundSystem( &soundSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Silent Sound System"
                );

            return false;
        }

        SoundServiceInterface * soundService;
        if( createSoundService( &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Application::initializeSilentSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSilentSoundEngine_ Failed to initialize Sound Engine"
                );

            return false;                
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeScriptEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Script Service..." );

        ScriptServiceInterface * scriptService;
        if( createScriptService( &scriptService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, scriptService ) == false )
        {
            return false;
        }

        m_scriptService = scriptService;

        if( m_scriptService->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeCodecEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

        CodecServiceInterface * codecService;
        if( createCodecService( &codecService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, codecService ) == false )
        {
            return false;
        }

        m_codecService = codecService;

        m_codecService->registerCodecExt( "png", Helper::stringizeString(m_serviceProvider, "pngImage") );
        m_codecService->registerCodecExt( "jpg", Helper::stringizeString(m_serviceProvider, "jpegImage") );
        m_codecService->registerCodecExt( "jpeg", Helper::stringizeString(m_serviceProvider, "jpegImage") );

        m_codecService->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );
        m_codecService->registerCodecExt( "hit", Helper::stringizeString(m_serviceProvider, "hitPick") );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeConverterEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

        ConverterServiceInterface * converterService;
        if( createConverterService( &converterService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, converterService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeInputEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Input Service..." );

        InputServiceInterface * inputService;
        if( createInputService( &inputService ) == false )
        {
            return false;
        }

        //bool result = m_inputEngine->initialize( m_serviceProvider );
        //if( result == false )
        //{
        //    MENGE_LOG_ERROR( "Input Engine initialization failed!" );
        //    return false;
        //}

        if( SERVICE_REGISTRY( m_serviceProvider, inputService ) == false )
        {
            return false;
        }

        m_inputService = inputService;

        if( m_inputService->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeAmplifierService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Amplifier Service..." );

        AmplifierServiceInterface * amplifierService;
        if( createAmplifierService( &amplifierService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, amplifierService ) == false )
        {
            return false;
        }

        m_amplifierService = amplifierService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const bool iOSApplication::Init( void )
    {
        ServiceProviderInterface * serviceProvider;
        if( createServiceProvider( &serviceProvider ) == false )
        {
            return false;
        }

        m_serviceProvider = serviceProvider;

        if( m_serviceProvider->registryService( "PlatformService", this ) == false )
        {
            return false;
        }

        m_serviceProvider = serviceProvider;


        AudioSessionInitialize(NULL, NULL, NULL, NULL);
        
        if( this->initializeStringizeService_() == false )
        {
            return false;
        }

        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        if( this->initializeNotificationService_() == false )
        {
            return false;
        }

        if( this->initializeUnicodeEngine_() == false )
        {
            return false;
        }

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }

        if( this->initializeThreadEngine_() == false )
        {
            return false;
        }

        if( this->initializeParticleEngine_() == false )
        {
            return false;
        }

        if( this->initializePhysicEngine2D_() == false )
        {
            return false;
        }

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }

        if( this->initializeSoundEngine_() == false )
        {
            if( this->initializeSilentSoundEngine_() == false )
            {
                return false;
            }

            return false;
        }

        if( this->initializeScriptEngine_() == false )
        {
            return false;
        }

        if( this->initializeCodecEngine_() == false )
        {
            return false;
        }

        if( this->initializeConverterEngine_() == false )
        {
            return false;
        }

        if( this->initializeInputEngine_() == false )
        {
            return false;
        }

        if( this->initializeAmplifierService_() == false )
        {
            return false;
        }

        if( this->initializeApplicationService_() == false )
        {
            return false;
        }

        String commandLine; //Empty

        if( m_application->setup( commandLine, m_settings.applicationSettings ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application setup failed" 
                );

            return false;
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Image Codec..." );

            initPluginMengeImageCodec( &m_pluginMengeImageCodec );
            m_pluginMengeImageCodec->initialize( m_serviceProvider );
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Sound Codec..." );

            initPluginMengeSoundCodec( &m_pluginMengeSoundCodec );
            m_pluginMengeSoundCodec->initialize( m_serviceProvider );
        }
               
        String languagePack;
        languagePack = m_settings.defaultLocale;

        if( m_application->createGame( m_settings.personalityModule, Helper::stringizeString(m_serviceProvider, languagePack), m_settings.resourcePacks, m_settings.languagePacks ) == false )
        {
            NSLog( @"Application create game failed"
                );

            return false;
        }

        String scriptInit;

        if( m_application->initializeGame( m_settings.appParams, scriptInit ) == false )
        {
            NSLog( @"application initGame failed!"
                );

            return false;
        }

        if( m_application->createRenderWindow( 0 ) == false )
        {
            NSLog( @"application createRenderWindow failed!"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::Frame( void )
    {
        if( m_application == 0 )
        {
            return;
        }

        bool rendered = false;

        if( m_application->isFocus() == true )
        {
            rendered = m_application->onRender();
        }

        float frameTime = m_timer.getDeltaTime();

        bool updating = m_application->onUpdate();

        if( updating == true )
        {
            m_application->onTick( frameTime );
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
            m_application->onFlush();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::Finalize( void )
    {
        if( m_application )
        {
            m_application->finalize();            
        }

        if( m_logService )
        {
            m_logService->unregisterLogger( m_loggerConsole );
            delete m_loggerConsole;
        }        

        destroyApplication( m_application );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::OpenAL_OtherAudioIsPlaying()
    {
        int playing;
        UInt32 size = sizeof(playing);
        AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &size, &playing);

        return (bool)playing;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::TurnSoundOn()
    {
        m_application->onTurnSound( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::TurnSoundOff()
    {        
        m_application->onTurnSound( false );        
    }
    //////////////////////////////////////////////////////////////////////////
    ApplicationInterface * iOSApplication::getApplication() const
    {
        return m_application;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::stop( void )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::getDesktopResolution( Resolution & _resolution ) const
    {
        _resolution = m_resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::getMaxClientResolution( Resolution & _resolution ) const override
    {
        return m_resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::minimizeWindow( void )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setHandleMouse( bool _handle )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setCursorPosition( int _x, int _y )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::openUrlInDefaultBrowser( const WString & _url )
    {
        //UnicodeToUtf8 utf8(_url);

        //[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString : utf8.c_str()]];
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSApplication::getShortPathName( const String & _name, char * _shortpath, size_t _shortpathlen )
    {
        strcpy( _shortpath, _name.c_str() );
        
        return _name.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::isDevelopmentMode() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    TimerInterface * iOSApplication::getTimer( void ) const
    {
        return ( TimerInterface * )&m_timer;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::showKeyboard( void )
    {
        [ pEAGLView showKeyboard ];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::hideKeyboard( void )
    {
        [ pEAGLView hideKeyboard ];
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterface * iOSApplication::loadDynamicLibrary( const FilePath & _filename )
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::unloadDynamicLibrary( DynamicLibraryInterface * _lib )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyVsyncChanged( bool _vsync )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyCursorModeChanged( bool _mode )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyCursorIconSetup( const FilePath & _filename, void * _buffer, size_t _size )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyCursorClipping( const Viewport & _viewport )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::notifyCursorUnClipping( void )
    {
    }
    //void iOSApplication::notifySoundInitialize()
    //{
    //    AudioSessionSetActive(true);
    //}
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::setAsScreensaver( bool _set )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::setupApplicationSetting_( FileServiceInterface * _fileService )
    {
        StartupConfigLoader loader(m_serviceProvider);

        if( loader.load( ConstString::none(), Helper::stringizeString(m_serviceProvider, "application.ini"), m_settings ) == false )
        {
            return false;
        }

        m_projectName = m_settings.projectName;
        m_companyName = m_settings.companyName;

        String platformName = "IPAD";

        m_settings.applicationSettings.platformName = platformName;
        m_settings.applicationSettings.baseDir = m_currentPath;

        //if( languagePack.empty() == true )
        //{
        //	WChar localeBuff[64];
        //	int localeBuffSize = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, localeBuff, 64 );
        //	WString languagePackW = std::wstring(localeBuff, localeBuffSize);

        //	WindowsLayer::unicodeToAnsi( languagePackW, languagePack );

        //	std::transform( languagePack.begin(), languagePack.end(), 
        //		languagePack.begin(), std::ptr_fun( &::tolower ) );
        //}

        return true;
    }
}
