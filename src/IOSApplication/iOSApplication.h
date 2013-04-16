/*
*  iOSApplication.h
*  Mengine
*
*  Created by Dgut on 9/18/11.
*  Copyright 2011 __MyCompanyName__. All rights reserved.
*
*/

#   pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterace.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/AmplifierServiceInterface.h"

#   include "Utils/StartupConfigLoader/StartupConfigLoader.h"

namespace Menge
{
    class FileServiceInterface;

    class iOSFileLogger 
        : public LoggerInterface
    {
    public:
        iOSFileLogger();

    public:
        void setVerboseLevel( EMessageLevel _level );
        bool validVerboseLevel( EMessageLevel _level ) const;

    public:
        void log( const char * _data, int _count, EMessageLevel _level );
        void flush( void );

    protected:
        EMessageLevel m_verboseLevel;
    };

    class iOSTimer: public TimerInterface
    {
        int									start;
        mutable unsigned long				oldTime;
    public:
        iOSTimer( void );

        virtual void reset( void );
        virtual float getDeltaTime( void ) const;
        virtual unsigned long getMilliseconds( void );
        virtual unsigned long getMicroseconds( void );
        virtual unsigned long getMillisecondsCPU( void );
        virtual unsigned long getMicrosecondsCPU( void );
    };

    class Application;

    class iOSApplication 
        : public PlatformInterface
    {
    public:
        iOSApplication( void );
        ~iOSApplication( void );

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        const bool Init( void );
        void Frame( void );
        void Finalize( void );

        bool OpenAL_OtherAudioIsPlaying();

        void TurnSoundOn();
        void TurnSoundOff();

    protected:
        bool initializeStringizeService_();
        bool initializeApplicationService_();
        bool initializeNotificationService_();
        bool initializeThreadEngine_();
        bool initializeFileEngine_();
        bool initializeLogEngine_();
        bool initializeUnicodeEngine_();
        bool initializeParticleEngine_();
        bool initializePhysicEngine2D_();
        bool initializeRenderEngine_();
        bool initializeSoundEngine_();
        bool initializeSilentSoundEngine_();
        bool initializeNodeManager_();
        bool initializeScriptEngine_();
        bool initializeCodecEngine_();
        bool initializeConverterEngine_();
        bool initializeInputEngine_();
        bool initializeAmplifierService_();

    public:
        ApplicationInterface * getApplication() const;

    public:
        const String & getCurrentPath() const override;
        size_t getShortPathName( const String & _name, char * _shortpath, size_t _shortpathlen ) override;

        bool isDevelopmentMode() const override;
        
    public:
        void stop( void ) override;
        void getDesktopResolution( Resolution & _resolution ) const override;
        void getMaxClientResolution( Resolution & _resolution ) const override;
        void minimizeWindow( void ) override;
        void setHandleMouse( bool _handle ) override;
        void setCursorPosition( int _x, int _y ) override;

        TimerInterface * getTimer( void ) const;

        void showKeyboard( void );
        void hideKeyboard( void );

        DynamicLibraryInterface * loadDynamicLibrary( const FilePath & _filename );
        void unloadDynamicLibrary( DynamicLibraryInterface * _lib );

        void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen );
        void notifyVsyncChanged( bool _vsync );
        void notifyCursorModeChanged( bool _mode );
        void notifyCursorIconSetup( const FilePath & _filename, void * _buffer, size_t _size ) override;

        void notifyCursorClipping( const Viewport & _viewport );
        void notifyCursorUnClipping( void );

        void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider );

        void setAsScreensaver( bool _set );

        bool openUrlInDefaultBrowser( const WString & _url ) override;
        
    protected:
        bool setupApplicationSetting_( FileServiceInterface * _fileService );

    public:
        bool setupLogService( LogServiceInterface * _logService );
        bool setupFileService( FileServiceInterface * _fileService );

    protected:
        iOSTimer m_timer;
        iOSFileLogger * m_loggerConsole;
        Resolution	m_resolution;

        ApplicationInterface * m_application;
        StartupSettings m_settings;

        ServiceProviderInterface * m_serviceProvider;

        InputServiceInterface * m_inputService;
        UnicodeServiceInterface * m_unicodeService;
        LogServiceInterface * m_logService;
        FileServiceInterface * m_fileService;
        CodecServiceInterface * m_codecService;
        ThreadServiceInterface * m_threadService;
        ParticleServiceInterface * m_particleService;
        PhysicService2DInterface * m_physicService2D;
        RenderServiceInterface * m_renderService;
        SoundServiceInterface * m_soundService;
        ScriptServiceInterface * m_scriptService;
        AmplifierServiceInterface * m_amplifierService;

        String m_userPath;
        String m_currentPath;
        
        WString m_projectName;
        WString m_companyName;

        PluginInterface * m_pluginMengeImageCodec;
        PluginInterface * m_pluginMengeSoundCodec;
    };

}
