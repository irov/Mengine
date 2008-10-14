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
//#	include <string>

#	include <SDL.h>

namespace Menge
{
	class SystemDLL;
	class LoggerConsole;
	class Application;

	class SDLApplication
		: public ApplicationInterface
	{
	public:
		SDLApplication( const StringA& _commandLine );
		~SDLApplication();

	public:
		//bool init( LogSystemInterface* _logSystem ) override;
		bool start();
		void loop();
		void stop() override;


		void createWindow(WindowHandle _handle);

		WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		//float getMonitorAspectRatio() override;
		std::size_t getDesktopWidth() const override;
		std::size_t getDesktopHeight() const override;

		void minimizeWindow() override;
		//void setPrimaryMonitorAspect( float _aspect );
		void setDesktopResolution( std::size_t _width, std::size_t _height );
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		float getDeltaTime() override;
		//void changeResolution( int _width, int _height, int _bits, bool _fullscreen );
		SystemDLLInterface* loadSystemDLL( const String & _dll ) override;
		void unloadSystemDLL( SystemDLLInterface* _interface ) override;
		void setHandleMouse( bool _handle ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;

	private:

		LARGE_INTEGER m_timerFrequency;
		LARGE_INTEGER m_lastTime;
		LARGE_INTEGER m_timer;

		Application* m_menge;
		LogSystemInterface* m_logSystem;

		bool	m_running;
		bool	m_active;
		float	m_frameTime;
		bool	m_cursorInArea;
		bool m_focus;
		String m_name;

		std::size_t m_windowWidth;
		std::size_t m_windowHeight;

		std::size_t m_desktopWidth;
		std::size_t m_desktopHeight;

		bool m_fullscreen;
		bool m_handleMouse;
		LoggerConsole* m_loggerConsole;
		StringA m_commandLine;
	};
}	// namespace Menge
