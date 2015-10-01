#	pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/MemoryInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/DataInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#	include "Interface/WindowsLayerInterface.h"
#	include "Interface/HttpSystemInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/ConfigInterface.h"


#	include "WinTimer.h"
#	include "WinMouseEvent.h"

#   include <stdex/stl_map.h>

namespace Menge
{
	class SystemDLL;
	class ConsoleLogger;
	class MessageBoxLogger;
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
		const ConstString & getPlatformName() const override;

	public:
		HWND createWindow( const Menge::WString & _name, const Resolution & _resolution, bool _fullscreen );

		void getDesktopResolution( Resolution & _resolution ) const override;

		size_t getCurrentPath( WChar * _path, size_t _len ) const override;

		void minimizeWindow() override;

		void setHandleMouse( bool _handle ) override;
		void setCursorPosition( const mt::vec2f & _pos ) override;

		TimerInterface * getTimer() const override;

		size_t getShortPathName( const WString & _name, WChar * _shortpath, size_t _len ) const override;
		void getMaxClientResolution( Resolution & _resolution ) const override;

		void showKeyboard() override;
		void hideKeyboard() override;

		void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
		void notifyVsyncChanged( bool _vsync ) override;
		void notifyCursorModeChanged( bool _mode ) override;
		bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const Blobject & _buffer ) override;

		void notifyCursorClipping( const Viewport & _viewport ) override;
		void notifyCursorUnClipping() override;

	public:
		void onEvent( const ConstString & _event, const TMapParams & _params ) override;

	public:
		bool openUrlInDefaultBrowser( const WString & _url ) override;		

	public:
		bool isSaverRunning() const;

		bool isDevelopmentMode() const override;
		bool isRoamingMode() const override;
		//void setAsScreensaver( bool _set );

	public:
		uint32_t getProcessHandleCount() const override;

	protected:
		bool initializeStringizeService_();
		bool initializeApplicationService_();
		bool initializeNotificationService_();
		bool initializeThreadEngine_();
		bool initializeFileEngine_();
		bool initializeUserDirectory_();
		bool initializeLogFile_();
		bool initializeLogEngine_();
		bool initializeConfigEngine_();
		bool initializeUnicodeEngine_();
		bool initializeParticleEngine2_();
		bool initializePhysicEngine2D_();
		bool initializeRenderEngine_();
		bool initializeSoundEngine_();
		bool initializeSilentSoundEngine_();
		bool initializeScriptEngine_();		
		bool initializeConverterEngine_();
		bool initializeCodecEngine_();
		bool initializeInputEngine_();
		bool initializePluginService_();
		bool initializeArchiveService_();
		bool initializeModuleEngine_();
		bool initializeDataManager_();
		bool initializeMemoryManager_();
		bool initializeHttpService_();
		bool initializePrefetcherService_();

	protected:
		bool getApplicationPath_( const char * _section, const char * _key, ConstString & _value );

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		bool wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & _result );

	protected:
		DWORD getWindowStyle( bool _fullsreen ) const;
		RECT getWindowsRect( const Resolution & _resolution, bool _fullsreen ) const;
		void setActive( bool _active );
		WChar translateVirtualKey_( unsigned int _vkc, unsigned int _vk );

	protected:
		bool calcCursorPosition( mt::vec2f & _point ) const;

	protected:
		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;

	protected:
		bool makeUserPath_( WString & _wstring ) const;

	private:
		WinTimer * m_winTimer;
		FPSMonitor * m_fpsMonitor;
		AlreadyRunningMonitor * m_alreadyRunningMonitor;

		ApplicationInterface * m_application;
		
		ConstString m_platformName;

		float m_pauseUpdatingTime;

		bool m_running;
		bool m_active;
		bool m_applicationUpdate;

		HWND m_hWnd;

		bool m_hasWindowPanel;
		bool m_cursorInArea;
		bool m_clickOutArea;
		HCURSOR m_cursor;

		WinMouseEvent m_mouseEvent;

		typedef stdex::map<ConstString, HCURSOR> TMapCursors;
		TMapCursors m_cursors;

		WString m_windowClassName;
		
		Resolution m_windowResolution;

		BOOL m_clipingCursor;
		RECT m_clipingCursorRect;

		HINSTANCE m_hInstance;

		ServiceProviderInterface * m_serviceProvider;

		InputServiceInterface * m_inputService;

		UnicodeSystemInterface * m_unicodeSystem;
		UnicodeServiceInterface * m_unicodeService;

		LogServiceInterface * m_logService;

		FileServiceInterface * m_fileService;

		CodecServiceInterface * m_codecService;
		ArchiveServiceInterface * m_archiveService;

		ThreadSystemInterface * m_threadSystem;
		ThreadServiceInterface * m_threadService;

		ParticleServiceInterface2 * m_particleService2;

		PhysicSystemInterface * m_physicSystem;

		RenderSystemInterface * m_renderSystem;

		StringizeServiceInterface * m_stringizeService;

		ConverterServiceInterface * m_converterService;
		NotificationServiceInterface * m_notificationService;

		SoundSystemInterface * m_soundSystem;
		SoundServiceInterface * m_soundService;

		ScriptServiceInterface * m_scriptService;
		PluginServiceInterface * m_pluginService;
		ModuleServiceInterface * m_moduleService;
		DataServiceInterface * m_dataService;
		MemoryServiceInterface * m_memoryService;
		HttpSystemInterface * m_httpSystem;
		ConfigServiceInterface * m_configService;

		PrefetcherServiceInterface * m_prefetcherService;

		FileLogger * m_fileLog;

		ConsoleLogger * m_loggerConsole;
		MessageBoxLogger * m_loggerMessageBox;
		String m_commandLine;

		bool m_lastMouse;
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
		bool m_roamingMode;
		bool m_nologsMode;
		bool m_noDevPluginsMode;
		bool m_profilerMode;
		bool m_muteMode;
		bool m_nopause;

		typedef stdex::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;	
	};
}	// namespace Menge
