#	pragma once

#	include "Config/Config.h"
#	include "Utils/Core/Resolution.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "WinIncluder.h"

namespace Menge
{
	
	class ParticlesLogger;
	class ScriptEngine;
	class RenderServiceInterface;
	class ScriptServiceInterface;
	class EmitterDescriptionManager;
	class RenderParticlesScriptWrapper;
	class FileOutputStreamInterface;
	struct RenderParticlesFrame;
	class TextureInterface;
	class RenderParticles:
		public PlatformInterface
	{
	public:
		////////////////////////////////////////////////////
		RenderParticles( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight );
		
		virtual void stop(){ }

		virtual const Resolution & getDesktopResolution() const{ return m_desktopResolution; }

		virtual void minimizeWindow() { }

		virtual void setHandleMouse( bool _handle ) { }
		virtual void setCursorPosition( int _x, int _y ) { }
		virtual void showMessageBox( const String& _message, const String& _header, unsigned int _style )  { }

		virtual TimerInterface * getTimer() const { return NULL; }

		virtual void showKeyboard(){ }
		virtual void hideKeyboard(){ }

		virtual void ansiToUtf8( const String& _ansi, String & _utf8 ){ }
		virtual void utf8ToAnsi( const String& _utf8, String & _ansi ){ }
		virtual void utf8Count( const String& _utf8, size_t & _size ){ }

		virtual DynamicLibraryInterface* loadDynamicLibrary( const String& _filename ){ return NULL; }
		virtual void unloadDynamicLibrary( DynamicLibraryInterface* _lib ){ }

		virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ){ }
		virtual void notifyVsyncChanged( bool _vsync ) { }
		virtual void notifyCursorModeChanged( bool _mode ){ }
		virtual void notifyCursorIconSetup( const String & _filename ){ }

		virtual void notifyCursorClipping( const Viewport & _viewport ){ }
		virtual void notifyCursorUnClipping(){ }
		virtual void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider ) {}
		virtual void notifySoundInitialize(){ }
		virtual void setAsScreensaver( bool _set ) { }
		
		bool initialise();
		void loop();
		void finalize();
		
		void beginRender();
		void endRender();
		void getRenderData( RenderParticlesFrame * _frame );
		void translateMessage();
		void log( const String& _message );
		ScriptServiceInterface * getScriptService();
	protected:
		bool initLogger_( FileOutputStreamInterface * _file );
		RECT getWindowsRect_( const Resolution & _resolution );
		HWND createWindow_( HINSTANCE hinstance, const Menge::String & _name, const Menge::String _className, const Resolution & _resolution );
		void calcRenderViewport_( Viewport & _viewport, const Resolution & _resolution );
		
		Resolution m_resolution;
		Resolution m_contentResolution;
		ConstString m_renderTargetName;
		Resolution m_windowResolution;
		Resolution m_desktopResolution;
		ApplicationInterface * m_application;
		
		RenderServiceInterface * m_renderService;
		ScriptServiceInterface * m_scriptService;
		TextureInterface * m_renderImage;
		TextureInterface * m_renderTargetImage;
		HWND	m_renderhWnd;
		HINSTANCE m_hInstance;
		ParticlesLogger * m_logger;
		LogServiceInterface * m_logService;
	};

}