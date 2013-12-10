#	pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterace.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#	include "Interface/WindowsLayerInterface.h"


#   include "Utils/StartupConfigLoader/StartupConfigLoader.h"

#	include "WinTimer.h"

#	include <map>

namespace Menge
{
	class SystemDLL;
	class ConsoleLogger;
	class Application;
	class FPSMonitor;
	class AlreadyRunningMonitor;

	class FileLogger;
	
	//////////////////////////////////////////////////////////////////////////
	class WinApplication
		: public PlatformInterface
	{
	public:
		WinApplication();
		~WinApplication();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( HINSTANCE _hInstance, const String& _commandLine );
		void finalize();

		void loop();
		void stop()	override;

	public:
		HWND createWindow( const Menge::WString & _name, const Resolution & _resolution, bool _fullscreen );

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

    public:
        void onEvent( const ConstString & _event, const TMapParams & _params ) override;

    public:
		bool openUrlInDefaultBrowser( const WString & _url ) override;
        bool cmd( const WString & _command ) override;

        size_t getMemoryUsage() const override;

        void * checkpointMemory() const override;
        size_t diffMemory( void * _checkpoint ) const override;
		
		bool isSaverRunning() const;

        bool isDevelopmentMode() const override;
		//void setAsScreensaver( bool _set );

    protected:
        bool initializeStringizeService_();
        bool initializeApplicationService_();
        bool initializeNotificationService_();
        bool initializeThreadEngine_();
        bool initializeFileEngine_();
        bool initializeUserDirectory_();
        bool initializeLogFile_();
        bool initializeLogEngine_();
        bool initializeUnicodeEngine_();
        bool initializeParticleEngine_();
        bool initializePhysicEngine2D_();
        bool initializeRenderEngine_();
        bool initializeSoundEngine_();
        bool initializeSilentSoundEngine_();
        bool initializeScriptEngine_();
		bool initializeModuleEngine_();
        bool initializeCodecEngine_();
        bool initializeConverterEngine_();
        bool initializeInputEngine_();
        bool initializePluginService_();
        bool initializeArchiveService_();

    protected:
        bool setupApplicationSetting_( StartupSettings & _settings );

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		bool wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & _result );

	protected:
		DWORD getWindowStyle( bool _fullsreen ) const;
		RECT getWindowsRect( const Resolution & _resolution, bool _fullsreen ) const;
		void setActive( bool _active );
		unsigned int translateVirtualKey_( unsigned int _vkc, unsigned int _vk );

	protected:
		void getCursorPosition( mt::vec2f & _point );

	private:
		WinTimer * m_winTimer;
		FPSMonitor * m_fpsMonitor;
		AlreadyRunningMonitor * m_alreadyRunningMonitor;

		ApplicationInterface * m_application;
        
        EWindowsType m_windowsType;

		bool m_running;
		bool m_active;

		HWND m_hWnd;
		bool m_hasWindowPanel;
		bool m_cursorInArea;
        bool m_clickOutArea;
		HCURSOR m_cursor;
		
		typedef std::map<WString, HCURSOR> TMapCursors;
		TMapCursors m_cursors;
		
		WString m_windowClassName;
		
		WString m_currentPath;
        WString m_userPath;
        WString m_tempPath;

        WString m_projectName;
        WString m_companyName;

		Resolution m_windowResolution;

		BOOL m_clipingCursor;
		RECT m_clipingCursorRect;

		HINSTANCE m_hInstance;

		ServiceProviderInterface * m_serviceProvider;

		InputServiceInterface * m_inputService;
        
        UnicodeSystemInterface * m_unicodeSystem;
        UnicodeServiceInterface * m_unicodeService;

		LogServiceInterface * m_logService;
        
        FileSystemInterface * m_fileSystem;
        FileServiceInterface * m_fileService;

        CodecServiceInterface * m_codecService;
        ArchiveServiceInterface * m_archiveService;

        ThreadSystemInterface * m_threadSystem;
        ThreadServiceInterface * m_threadService;

        ParticleSystemInterface * m_particleSystem;
        ParticleServiceInterface * m_particleService;

        PhysicService2DInterface * m_physicService2D;

        RenderSystemInterface * m_renderSystem;
        RenderServiceInterface * m_renderService;
        RenderTextureServiceInterface * m_renderTextureManager;
        RenderMaterialServiceInterface * m_renderMaterialManager;

        StringizeServiceInterface * m_stringizeService;

        ConverterServiceInterface * m_converterService;
        NotificationServiceInterface * m_notificationService;

        SoundSystemInterface * m_soundSystem;
        SoundServiceInterface * m_soundService;

        ScriptServiceInterface * m_scriptService;
        PluginServiceInterface * m_pluginService;
		ModuleServiceInterface * m_moduleService;

		FileLogger * m_fileLog;

		ConsoleLogger* m_loggerConsole;
		String m_commandLine;

		int m_lastMouseX;
		int m_lastMouseY;

        WindowsLayerInterface * m_windowsLayer;
		
		bool m_vsync;

		bool m_maxfps;
		bool m_allowMaximize;
		bool m_cursorMode;
		WChar m_deadKey;

		bool m_isDoubleClick;

		bool m_enableDebug;
		bool m_developmentMode;
        bool m_noDevPluginsMode;
        bool m_profilerMode;
        bool m_muteMode;

        PluginInterface * m_pluginMengeImageCodec;
        PluginInterface * m_pluginMengeSoundCodec;
	};
}	// namespace Menge
