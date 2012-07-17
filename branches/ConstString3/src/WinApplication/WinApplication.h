#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include "WindowsLayer/WindowsLayer.h"

#	include "WinTimer.h"

#	include <map>

namespace Menge
{
	class SystemDLL;
	class LoggerConsole;
	class Application;
	class FPSMonitor;
	class AlreadyRunningMonitor;

	class FileLogger;
	
	class LogServiceInterface;
	class FileServiceInterface;
	class InputServiceInterface;

	//////////////////////////////////////////////////////////////////////////
	class WinApplication
		: public PlatformInterface
	{
	public:
		WinApplication( HINSTANCE _hInstance, const String& _commandLine );
		~WinApplication();

	public:
		bool initialize();
		void finalize();

		void loop();
		void stop()	override;

	public:
		WindowHandle createWindow( const Menge::WString & _name, const Resolution & _resolution, bool _fullscreen );

		const Resolution & getDesktopResolution() const override;

		const WString & getCurrentPath() const override;

		void minimizeWindow() override;
		void setDesktopResolution( const Resolution & _resolution );
		void setHandleMouse( bool _handle ) override;
		void setCursorPosition( int _x, int _y ) override;
		
		void showMessageBox( const WString& _message, const WString& _caption, unsigned int _style ) override;

		TimerInterface * getTimer() const override;
		
		void showKeyboard() override;
		void hideKeyboard() override;

		DynamicLibraryInterface* loadDynamicLibrary( const WString& _filename ) override;
		void unloadDynamicLibrary( DynamicLibraryInterface* _lib ) override;

		void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
		void notifyVsyncChanged( bool _vsync ) override;
		void notifyCursorModeChanged( bool _mode ) override;
		void notifyCursorIconSetup( const ConstString & _name, void * _buffer, size_t _size ) override;

		void notifyCursorClipping( const Viewport & _viewport ) override;
		void notifyCursorUnClipping() override;

		void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider ) override;

		static bool isSaverRunning();
		void setAsScreensaver( bool _set );

	public:
		void setupLogService( LogServiceInterface * _service );
		void setupFileService( FileServiceInterface * _service );
		void setupInputService( InputServiceInterface * _service );

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		void wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		DWORD getWindowStyle( bool _fullsreen );
		RECT getWindowsRect( const Resolution & _resolution, bool _fullsreen );
		void setActive( bool _active );
		unsigned int translateVirtualKey_( unsigned int _vkc, unsigned int _vk );

	protected:
		void getCursorPosition( mt::vec2f & _point );

	private:
		WinTimer * m_winTimer;
		FPSMonitor * m_fpsMonitor;
		AlreadyRunningMonitor * m_alreadyRunningMonitor;

		ApplicationInterface * m_application;
		bool	m_running;
		bool	m_active;

		HWND	m_hWnd;
		bool	m_hasWindowPanel;
		bool	m_cursorInArea;
		HCURSOR m_cursor;
		
		typedef std::map<ConstString, HCURSOR> TMapCursors;
		TMapCursors m_cursors;
		
		WString m_name;

		WString m_windowClassName;
		
		WString m_currentPath;

		WString m_applicationPath;
		WString m_userPath;

		Resolution m_windowResolution;
		Resolution m_desktopResolution;

		BOOL m_clipingCursor;
		RECT m_clipingCursorRect;

		HINSTANCE m_hInstance;

		ServiceProviderInterface * m_serviceProvider;

		InputServiceInterface * m_inputService;

		LogServiceInterface * m_logService;
		FileLogger * m_fileLog;

		LoggerConsole* m_loggerConsole;
		String m_commandLine;

		int m_lastMouseX;
		int m_lastMouseY;

		WindowsLayer::EWindowsType m_windowsType;
		
		bool m_vsync;

		bool m_maxfps;
		bool m_allowMaximize;
		bool m_cursorMode;
		wchar_t m_deadKey;

		bool m_isDoubleClick;

		bool m_enableDebug;
	};
}	// namespace Menge
