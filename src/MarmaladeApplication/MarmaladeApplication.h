#pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterface.h"
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
		const ConstString & getPlatformName() const override;

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

        void onEvent( const ConstString & _event, const TMapParams & _params ) override;

	public:
		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;

	public:
		uint32_t getProcessHandleCount() const override;

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
        bool initializeInputEngine_();        
        bool initializePluginService_();
        bool initializeArchiveService_();
		bool initializeModuleEngine_();		
		bool initializeDataManager_();
		bool initializeCacheManager_();
		bool initializePrefetcherService_();

	public:
		void setActivate( bool _value );

	protected:
		void initializeMarmaladePauseCallback_();
				
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ApplicationInterface * m_application;
		
		MarmaladeLayerInterface * m_marmaladeLayer;

        MarmaladeLogger * m_loggerConsole;
		FileLogger * m_fileLog;

		MarmaladeTimer * m_timer;
		MarmaladeInput * m_marmaladeInput;

		String m_commandLine;
		
		ConstString m_platformName;

        FilePath m_currentPath;
        WString m_userPath;
        WString m_tempPath;

        WString m_projectName;
        WString m_companyName;
		
        InputServiceInterface * m_inputService;
		UnicodeSystemInterface * m_unicodeSystem;
        UnicodeServiceInterface * m_unicodeService;
        LogServiceInterface * m_logService;
        FileServiceInterface * m_fileService;
        CodecServiceInterface * m_codecService;
		ThreadSystemInterface * m_threadSystem;
        ThreadServiceInterface * m_threadService;
        ParticleServiceInterface * m_particleService;
		RenderSystemInterface * m_renderSystem;
        RenderServiceInterface * m_renderService;
		RenderTextureServiceInterface * m_renderTextureManager;
		RenderMaterialServiceInterface * m_renderMaterialManager;
		SoundSystemInterface * m_soundSystem;
        SoundServiceInterface * m_soundService;
        ScriptServiceInterface * m_scriptService;
        ArchiveServiceInterface * m_archiveService;
		ModuleServiceInterface * m_moduleService;
		DataServiceInterface * m_dataService;
		CacheServiceInterface * m_cacheService;
		ConfigServiceInterface * m_configService;
		PrefetcherServiceInterface * m_prefetcherService;
		NotificationServiceInterface * m_notificationService;
		StringizeServiceInterface * m_stringizeService;

		typedef stdex::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;	

		bool m_running;
		bool m_active;

		bool m_developmentMode;
    };
}
