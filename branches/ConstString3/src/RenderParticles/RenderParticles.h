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
	class RenderTextureInterface;

	class RenderParticles:
		public PlatformInterface
	{
	public:
		////////////////////////////////////////////////////
		RenderParticles( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight );
		//dummy interface
		virtual void stop(){ }

		virtual const Resolution & getDesktopResolution() const{ return m_desktopResolution; }
		virtual const WString & getCurrentPath() const {  return m_currentPath;  }

		virtual void minimizeWindow() { }

		virtual void setHandleMouse( bool _handle ) { }
		virtual void setCursorPosition( int _x, int _y ) { }
		virtual void showMessageBox( const WString& _message, const WString& _header, unsigned int _style )  { }

		virtual TimerInterface * getTimer() const { return NULL; }

		virtual void showKeyboard(){ }
		virtual void hideKeyboard(){ }
		
		//deprecated
		virtual String ansiToUtf8( const String& _ansi ) ;
		virtual String utf8ToAnsi( const String& _utf8 ) ;
		virtual size_t utf8Count( const String& _utf8 ) ;
		virtual String unicodeToAnsi( const WString& _unicode ) ;
		virtual WString ansiToUnicode( const String& _utf8 ) ;
		virtual WString utf8ToUnicode( const String & _utf8 ) ;
		//end deprecated
		
		virtual DynamicLibraryInterface* loadDynamicLibrary( const WString& _filename ){ return NULL; }
		virtual void unloadDynamicLibrary( DynamicLibraryInterface* _lib ){ }

		virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ){ }
		virtual void notifyVsyncChanged( bool _vsync ) { }
		virtual void notifyCursorModeChanged( bool _mode ){ }
		virtual void notifyCursorIconSetup( const ConstString & _name, void * _buffer, size_t _size ){ }

		virtual void notifyCursorClipping( const Viewport & _viewport ){ }
		virtual void notifyCursorUnClipping(){ }
		virtual void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider ) {}
		virtual void notifySoundInitialize(){ }
		virtual void setAsScreensaver( bool _set ) { }
		bool openUrlInDefaultBrowser( const WString & _url ) override;

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
		HWND createWindow_( HINSTANCE hinstance, const Menge::WString & _name, const Menge::WString _className, const Resolution & _resolution );
		void calcRenderViewport_( Viewport & _viewport, const Resolution & _resolution );
		
		Resolution m_resolution;
		Resolution m_contentResolution;
		ConstString m_renderTargetName;
		Resolution m_windowResolution;
		Resolution m_desktopResolution;
		ApplicationInterface * m_application;
		
		// only for interface instantiation
		WString m_currentPath;
		
		RenderServiceInterface * m_renderService;
		ScriptServiceInterface * m_scriptService;
		RenderTextureInterface * m_renderImage;
		RenderTextureInterface * m_renderTargetImage;
		HWND	m_renderhWnd;
		HINSTANCE m_hInstance;
		ParticlesLogger * m_logger;
		LogServiceInterface * m_logService;
	};

}