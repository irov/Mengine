#pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterace.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#	include "Interface/PhysicSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#	include "Interface/ArchiveInterface.h"
#   include "Interface/MarmaladeLayerInterface.h"
#	include "Interface/ModuleInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/CacheInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/PrefetcherInterface.h"

#	include "MarmaladeLogger.h"
#   include "MarmaladeTimer.h"
#   include "MarmaladeInput.h"

#	include <map>

namespace Menge
{
    //    class LoggerConsole;
    class Application;
    class FPSMonitor;
    class AlreadyRunningMonitor;

    class FileLogger;

    class MarmaladeApplication
        : public PlatformInterface
    {
    public:
        MarmaladeApplication();
        ~MarmaladeApplication();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const String & _commandLine );
        void finalize();

        void loop();
        void stop()	override;

    public:
        void getDesktopResolution( Resolution & _resolution ) const override;

        const String & getCurrentPath() const override;

        void minimizeWindow() override;

        void setHandleMouse( bool _handle ) override;
        void setCursorPosition( const mt::vec2f & _pos ) override;

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

        bool isDevelopmentMode() const override;

        void onEvent( const ConstString & _event, const TMapParams & _params );

	public:
		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;

    public:
        size_t getMemoryUsage() const override;
        void * checkpointMemory() const override;
        size_t diffMemory( void * _checkpoint ) const override;

	protected:
		bool getApplicationPath_( const char * _section, const char * _key, ConstString & _path );

    protected:
        bool initializeStringizeService_();
        bool initializeApplicationService_();
        bool initializeNotificationService_();
        bool initializeThreadEngine_();
        bool initializeFileEngine_();
		bool initializeConfigEngine_();
        bool initializeLogEngine_();
        bool initializeMarmaladeLayerService_();
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
        bool initializePluginService_();
        bool initializeArchiveService_();
		bool initializeModuleEngine_();		
		bool initializeDataManager_();
		bool initializeCacheManager_();
		bool initializePrefetcherService_();
				
    private:
        MarmaladeLogger * m_loggerConsole;
        ApplicationInterface * m_application;

        bool	m_running;
        bool	m_active;

        FilePath m_currentPath;
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
        RenderServiceInterface * m_renderService;
		PhysicSystemInterface * m_physicSystem;
		RenderTextureServiceInterface * m_renderTextureManager;
		RenderMaterialServiceInterface * m_renderMaterialManager;
        SoundServiceInterface * m_soundService;
        ScriptServiceInterface * m_scriptService;
        PluginServiceInterface * m_pluginService;
        ArchiveServiceInterface * m_archiveService;
		ModuleServiceInterface * m_moduleService;
		DataServiceInterface * m_dataService;
		CacheServiceInterface * m_cacheService;
		ConfigServiceInterface * m_configService;
		PrefetcherServiceInterface * m_prefetcherService;


        MarmaladeLayerInterface * m_marmaladeLayer; 

        FileLogger * m_fileLog;

        String m_commandLine;

        MarmaladeTimer * m_timer;
        MarmaladeInput * m_input;

		PluginInterface * m_pluginMengeXmlCodec;
        PluginInterface * m_pluginMengeImageCodec;
        PluginInterface * m_pluginMengeSoundCodec;
		PluginInterface * m_pluginMengeAmplifier;
		PluginInterface * m_pluginMengeZip;
		PluginInterface * m_pluginMengeLZ4;
		PluginInterface * m_pluginPluginPathFinder;
    };
}
