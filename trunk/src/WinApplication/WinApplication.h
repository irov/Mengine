#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include "WindowsIncluder.h"

#	include "WinTimer.h"

namespace Menge
{
	class SystemDLL;
	class LoggerConsole;
	class Application;
	class FPSMonitor;
	class AlreadyRunningMonitor;

	enum EWindowsType
	{
		EWT_NT
		, EWT_98
		, EWT_VISTA
	};

	//////////////////////////////////////////////////////////////////////////
	class WinApplication
		: public ApplicationInterface
	{
	public:
		WinApplication( HINSTANCE _hInstance, const String& _commandLine );
		~WinApplication();

	public:
		bool initialize();
		void finialize();

		void loop();
		void stop()	override;

		WindowHandle createWindow( const Menge::String & _name, const Resolution & _resolution, bool _fullscreen );
		const Resolution & getDesktopResolution() const override;

		void minimizeWindow() override;
		void setDesktopResolution( const Resolution & _resolution );
		void setHandleMouse( bool _handle ) override;
		void setCursorPosition( int _x, int _y ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;
		TimerInterface * getTimer() const override;
		String ansiToUtf8( const String& _ansi ) override;
		String utf8ToAnsi( const String& _utf8 ) override;

		DynamicLibraryInterface* load( const String& _filename ) override;
		void unload( DynamicLibraryInterface* _lib ) override;

		void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
		void notifyVsyncChanged( bool _vsync ) override;
		void notifyCursorModeChanged( bool _mode ) override;

		void notifyCursorClipping( const Viewport & _viewport ) override;
		void notifyCursorUnClipping() override;

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	protected:
		DWORD getWindowStyle( bool _fullsreen );
		RECT getWindowsRect( const Resolution & _resolution, bool _fullsreen );

	private:
		WinTimer * m_winTimer;
		FPSMonitor * m_fpsMonitor;
		AlreadyRunningMonitor * m_alreadyRunningMonitor;

		Application* m_application;
		bool	m_running;
		bool	m_active;
		float	m_frameTime;
		HWND	m_hWnd;
		bool	m_hasWindowPanel;
		bool	m_cursorInArea;
		WINDOWINFO m_wndInfo;
		String m_name;

		Resolution m_windowResolution;
		Resolution m_desktopResolution;

		bool m_handleMouse;
		BOOL m_clipingCursor;
		RECT m_clipingCursorRect;

		HINSTANCE m_hInstance;

		LoggerConsole* m_loggerConsole;
		String m_commandLine;

		int m_lastMouseX;
		int m_lastMouseY;

		EWindowsType m_windowsType;
		
		bool m_vsync;

		bool m_maxfps;
		bool m_allowMaximize;
		bool m_cursorMode;
	};
}	// namespace Menge
