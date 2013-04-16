#pragma once

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
#   include "Interface/PluginInterface.h"


#   include "Utils/StartupConfigLoader/StartupConfigLoader.h"
#	include "MarmaladeLogger.h"

#	include <map>

namespace Menge
{

//    class LoggerConsole;
        class Application;
        class FPSMonitor;
        class AlreadyRunningMonitor;

    class FileLogger;

    class MarmaladeApplication
            :public PlatformInterface
    {
        public:
            MarmaladeApplication();
            ~MarmaladeApplication();

        public:
            void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
            ServiceProviderInterface * getServiceProvider() const override;

        public:
            bool initialize();
            void finalize();

            void loop();
            void stop()	override;

        public:

            void getDesktopResolution( Resolution & _resolution ) const override;

            const String & getCurrentPath() const override;

            void minimizeWindow() override;

            void setHandleMouse( bool _handle ) override;
            void setCursorPosition( int _x, int _y ) override;

            TimerInterface * getTimer() const override;

            size_t getShortPathName( const String & _name, char * _shortpath, size_t _shortpathlen ) override;
            void getMaxClientResolution( Resolution & _resolution ) const override;

            void showKeyboard() override;
            void hideKeyboard() override;

            void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
            void notifyVsyncChanged( bool _vsync ) override;
            void notifyCursorModeChanged( bool _mode ) override;
            void notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size ) override;

            void notifyCursorClipping( const Viewport & _viewport ) override;
            void notifyCursorUnClipping() override;

            bool openUrlInDefaultBrowser( const WString & _url ) override;

//            bool isSaverRunning() const;

            bool isDevelopmentMode() const override;
            //void setAsScreensaver( bool _set );

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
            bool initializePluginService_();

        protected:
            bool setupApplicationSetting_();

        protected:
//            void getCursorPosition( mt::vec2f & _point );

        private:
//            Timer * m_Timer;
//            FPSMonitor * m_fpsMonitor;
//            AlreadyRunningMonitor * m_alreadyRunningMonitor;

			MarmaladeLogger m_loggerConsole;
            ApplicationInterface * m_application;
            StartupSettings m_settings;

            bool	m_running;
            bool	m_active;

            WString m_currentPath;
            WString m_userPath;
            WString m_tempPath;

            WString m_projectName;
            WString m_companyName;

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
            PluginServiceInterface * m_pluginService;

            FileLogger * m_fileLog;

            String m_commandLine;

//            bool m_vsync;

//            bool m_maxfps;
//            bool m_allowMaximize;
//            bool m_cursorMode;
//            WChar m_deadKey;

//            bool m_isDoubleClick;

//            bool m_enableDebug;
//            bool m_developmentMode;
//            bool m_noPluginMode;

//            PluginInterface * m_pluginMengeImageCodec;
//            PluginInterface * m_pluginMengeSoundCodec;
        };
}
