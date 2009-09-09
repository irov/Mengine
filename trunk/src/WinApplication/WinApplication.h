#	pragma once

/*#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0410		// Change this to the appropriate value to target other versions of Windows.
#endif*/

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0410	// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#	include "Interface/ApplicationInterface.h"
#	include <Windows.h>

#	include "WinTimer.h"

namespace Menge
{
	class SystemDLL;
	class LoggerConsole;
	class Application;

	class WinApplication
		: public ApplicationInterface
	{
	public:
		WinApplication( HINSTANCE _hInstance, const String& _commandLine );
		~WinApplication();

	public:
		bool start();
		void loop();
		void stop()	override;

		WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen, bool _hasPanel );
		std::size_t getDesktopWidth() const override;
		std::size_t getDesktopHeight() const override;

		void minimizeWindow() override;
		void setDesktopResolution( std::size_t _width, std::size_t _height );
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		void setHandleMouse( bool _handle ) override;
		void setCursorPosition( int _x, int _y ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;
		TimerInterface * getTimer() const override;
		String ansiToUtf8( const String& _ansi ) override;
		String utf8ToAnsi( const String& _utf8 ) override;

		DynamicLibraryInterface* load( const String& _filename ) override;
		void unload( DynamicLibraryInterface* _lib ) override;

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		DWORD threadFrameSignal();

	private:
		WinTimer * m_winTimer;

		Application* m_menge;
		LogSystemInterface* m_logSystem;
		bool	m_running;
		bool	m_active;
		float	m_frameTime;
		HWND	m_hWnd;
		bool	m_hasWindowPanel;
		bool	m_cursorInArea;
		WINDOWINFO m_wndInfo;
		HANDLE m_mutex;	// for multiple instance tracking
		bool m_focus;
		String m_name;
		HANDLE m_hEvent;
		HANDLE m_hThread;

		std::size_t m_windowWidth;
		std::size_t m_windowHeight;

		std::size_t m_desktopWidth;
		std::size_t m_desktopHeight;

		bool m_fullscreen;
		bool m_handleMouse;
		HINSTANCE m_hInstance;
		LoggerConsole* m_loggerConsole;
		String m_commandLine;

		unsigned long m_frameTiming;

		int m_lastMouseX;
		int m_lastMouseY;
		bool m_vsync;
		bool m_maxfps;
		bool m_allowMaximize;
	};
}	// namespace Menge
