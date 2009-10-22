/*
 *  MacOSApplication.h
 *  Mac
 *
 *  Created by Berserk on 22.10.08.
 *  Copyright 2008 __Menge__. All rights reserved.
 *
 */

#pragma once

#include "Interface/ApplicationInterface.h"
#include <Carbon/Carbon.h>

namespace Menge
{
	class Application;
	class OSXTimer;
	class LoggerConsole;
	
	class MacOSApplication
		: public ApplicationInterface
	{
	public:
		MacOSApplication( const StringA& _commandLine );
		~MacOSApplication();
		
	public:
		bool start();
		void loop();
		void stop() override;
		
		TimerInterface* getTimer() const;
		WindowHandle createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		std::size_t getDesktopWidth() const override;
		std::size_t getDesktopHeight() const override;

		void minimizeWindow() override;
		void notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		float getDeltaTime() override;
		//void changeResolution( int _width, int _height, int _bits, bool _fullscreen );
		void setHandleMouse( bool _handle ) override;
		void showMessageBox( const String& _message, const String& _header, unsigned int _style ) override;
		String ansiToUtf8( const String& _ansi ) override;
		String utf8ToAnsi( const String& _utf8 ) override;	

		OSStatus windowHandler( EventHandlerCallRef nextHandler, EventRef event );
		OSStatus clientHandler( EventHandlerCallRef nextHandler, EventRef event );

		DynamicLibraryInterface* load( const String& _filename ) override;
		void unload( DynamicLibraryInterface* _interface ) override;

		void setCursorPosition(int _x, int _y) override;
		
	public:
		static OSStatus s_windowHandler( EventHandlerCallRef nextHandler, EventRef event, void* params );
		static OSStatus s_clientHandler( EventHandlerCallRef nextHandler, EventRef event, void* params );

	protected:
		WindowRef createWindow_( const String& _title, int _width, int _height, bool _fullscreen, bool _hasWindowPanel );
		
	protected:
		StringA m_commandLine;
		Application* m_menge;
		LogSystemInterface* m_logSystem;
		bool m_running;
		bool m_handleMouse;
		OSXTimer* m_timer;
		LoggerConsole* m_loggerConsole;
		WindowRef m_window;
		HIViewRef m_view;
		EventHandlerUPP m_windowHandlerUPP;
		EventHandlerRef m_windowEventHandler;
		std::size_t m_desktopResolution[2];
		IBNibRef m_nibRef;
		bool m_fullscreenOverride;
		bool m_focus;
	};
}	// namespace Menge
		