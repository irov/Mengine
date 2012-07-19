#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class LogServiceInterface;
	class ServiceProviderInterface;

	class TimerInterface
	{
	public:
		virtual void reset() = 0;
		virtual float getDeltaTime() const = 0;

	public:
		virtual unsigned long getMilliseconds() = 0;
		virtual unsigned long getMicroseconds() = 0;
		virtual unsigned long getMillisecondsCPU() = 0;
		virtual unsigned long getMicrosecondsCPU() = 0;
	};

	class ConsoleInterface 
	{
	public:
		virtual void render() = 0;
		virtual void proccessInput( unsigned int _key, unsigned int _char, bool _isDown ) = 0;
	};

	class DynamicLibraryInterface;

	class PlatformInterface
	{
	public:
		virtual void stop() = 0;

		virtual const Resolution & getDesktopResolution() const = 0;
		virtual const WString & getCurrentPath() const = 0;

		virtual void minimizeWindow() = 0;

		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void setCursorPosition( int _x, int _y ) = 0;
		virtual void showMessageBox( const WString& _message, const WString& _header, unsigned int _style ) = 0;

		virtual TimerInterface * getTimer() const = 0;

		virtual void showKeyboard() = 0;
		virtual void hideKeyboard() = 0;

		virtual DynamicLibraryInterface* loadDynamicLibrary( const WString& _filename ) = 0;
		virtual void unloadDynamicLibrary( DynamicLibraryInterface* _lib ) = 0;

		virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void notifyVsyncChanged( bool _vsync ) = 0;
		virtual void notifyCursorModeChanged( bool _mode ) = 0;
		virtual void notifyCursorIconSetup( const ConstString & _name, void * _buffer, size_t _size ) = 0;

		virtual void notifyCursorClipping( const Viewport & _viewport ) = 0;
		virtual void notifyCursorUnClipping() = 0;

		//virtual void notifySoundInitialize() = 0;

		virtual void notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider ) = 0;

		virtual void setAsScreensaver( bool _set ) = 0;
	};

	class ApplicationInterface
		: public ServiceInterface
	{
	public:
		virtual bool initialize( PlatformInterface* _interface, const String & _platformName, const String& _args, const WString & _baseDir, const WString & _settingFile ) = 0;
		virtual bool loadConfig( const WString& _iniFile ) = 0;

		virtual void setBaseDir( const WString& _baseDir ) = 0;
		virtual const WString& getBaseDir() const = 0;
		
	public:
		virtual PlatformInterface * getPlatform() const = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:		
		virtual void setDesktopResolution( const Resolution & _resolution ) = 0;

		virtual void setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight ) = 0;

	public:
		virtual bool getHasWindowPanel() const = 0;
		virtual bool getAllowFullscreenSwitchShortcut() const = 0;

	public:
		virtual void setLanguagePackOverride( const ConstString & _packName ) = 0;

	public:
		virtual bool onRender() = 0;
		virtual void onFlush() = 0;
		virtual bool onUpdate() = 0;
		virtual void onTick( float _timing ) = 0;
		virtual void onFocus( bool _focus, const mt::vec2f & _point ) = 0;
		virtual void onClose() = 0;

		virtual void onTurnSound( bool _turn ) = 0;

		virtual void onAppMouseEnter( const mt::vec2f & _point ) = 0;
		virtual void onAppMouseLeave() = 0;

		virtual void onPaint() = 0;

	public:		
		virtual bool createGame() = 0;
		virtual bool loadGameResource() = 0;
		virtual bool initializeGame( const String & _scriptInitParams ) = 0;

		virtual bool loadPlugins() = 0;
		virtual bool loadPersonality() = 0;

	public:
		virtual const WString & getProjectTitle() const = 0;
		virtual const ConstString & getProjectName() const = 0;

	public:
		virtual int getAlreadyRunningPolicy() const = 0;

	public:
		virtual void setFullscreenMode( bool _fullscreen ) = 0;
		virtual bool getFullscreenMode() const = 0;

	public:
		virtual void calcWindowResolution( Resolution & _windowResolution ) const = 0;

		virtual const Resolution & getDesktopResolution() const = 0;
		virtual const Resolution & getCurrentResolution() const = 0;
		virtual const Viewport & getRenderViewport() const = 0;
		
		virtual const Resolution & getContentResolution() const = 0;
		virtual const Viewport & getLowContentViewport() const = 0;

	public:
		virtual bool createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle ) = 0;

	public:
		virtual bool getVSync() const = 0;
		virtual bool isFocus() const = 0;

	public:
		virtual void finalize() = 0;
		virtual void destroy() = 0;

	public:
		virtual const WString& getScreensaverName() const = 0;
	};
	
	typedef void* (*TDynamicLibraryFunction)(ApplicationInterface * _interface);

	class DynamicLibraryInterface
    {
    public:
        virtual bool load() = 0;
        virtual void unload() = 0;

	public:
		virtual const WString& getName() const = 0;

	public:
        virtual TDynamicLibraryFunction getSymbol( const String& _name ) const = 0;
    };
}

bool initInterfaceSystem( Menge::ApplicationInterface**	_interface );
void releaseInterfaceSystem( Menge::ApplicationInterface* _interface );
