#	pragma once

/*#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif*/

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
		WinApplication( HINSTANCE _hInstance, const StringA& _commandLine );
		~WinApplication();

	public:
		bool start();
		void loop();
		void stop()	override;

		void createWindow(WindowHandle _handle);

		WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		std::size_t getDesktopWidth() const override;
		std::size_t getDesktopHeight() const override;

		void minimizeWindow() override;
		void setDesktopResolution( std::size_t _width, std::size_t _height );
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		SystemDLLInterface* loadSystemDLL( const String & _dll ) override;
		void unloadSystemDLL( SystemDLLInterface* _interface ) override;
		void setHandleMouse( bool _handle ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;
		TimerInterface * getTimer() const override;

	public:
		LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		DWORD threadFrameSignal();

	private:
		WinTimer * m_winTimer;

		Application* m_menge;
		LogSystemInterface* m_logSystem;
		LARGE_INTEGER m_timerFrequency;
		LARGE_INTEGER m_lastTime;
		bool	m_running;
		bool	m_active;
		float	m_frameTime;
		LARGE_INTEGER m_timer;
		HWND	m_hWnd;
		bool	m_cursorInArea;
		WINDOWINFO m_wndInfo;
		HANDLE m_mutex;	// for multiple instance tracking
		bool m_focus;
		String m_name;
		HANDLE m_hEvent;

		std::size_t m_windowWidth;
		std::size_t m_windowHeight;

		std::size_t m_desktopWidth;
		std::size_t m_desktopHeight;

		bool m_fullscreen;
		bool m_handleMouse;
		HINSTANCE m_hInstance;
		LoggerConsole* m_loggerConsole;
		StringA m_commandLine;

		unsigned long m_frameTiming;
	};
}	// namespace Menge
