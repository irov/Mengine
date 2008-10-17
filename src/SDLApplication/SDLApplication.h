#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include <SDL.h>
#	include <map>
namespace Menge
{
	class SystemDLL;
	class Application;

	class SDLApplication
		: public ApplicationInterface
	{
	public:
		SDLApplication( const StringA& _commandLine );
		~SDLApplication();

	public:
		bool start();
		void loop();
		void stop() override;

		std::size_t getDesktopWidth() const override;
		std::size_t getDesktopHeight() const override;

		WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) override;

		void minimizeWindow() override;
		void setDesktopResolution( std::size_t _width, std::size_t _height );
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		float getDeltaTime() override;
		SystemDLLInterface* loadSystemDLL( const String & _dll ) override;
		void unloadSystemDLL( SystemDLLInterface* _interface ) override;
		void setHandleMouse( bool _handle ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;

	private:
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
		StringA m_commandLine;
	};
}	// namespace Menge
