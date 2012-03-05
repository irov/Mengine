#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class LogServiceInterface;
	class ServiceProviderInterface;

	class TimerInterface
	{
	public:
		virtual void reset() = 0;
		virtual float getDeltaTime() const = 0;
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

		virtual void minimizeWindow() = 0;

		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void setCursorPosition( int _x, int _y ) = 0;
		virtual void showMessageBox( const String& _message, const String& _header, unsigned int _style ) = 0;

		virtual TimerInterface * getTimer() const = 0;

		virtual void showKeyboard() = 0;
		virtual void hideKeyboard() = 0;

		virtual void ansiToUtf8( const String& _ansi, String & _utf8 ) = 0;
		virtual void utf8ToAnsi( const String& _utf8, String & _ansi ) = 0;
		virtual void utf8Count( const String& _utf8, size_t & _size ) = 0;

		virtual DynamicLibraryInterface* loadDynamicLibrary( const String& _filename ) = 0;
		virtual void unloadDynamicLibrary( DynamicLibraryInterface* _lib ) = 0;

		virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void notifyVsyncChanged( bool _vsync ) = 0;
		virtual void notifyCursorModeChanged( bool _mode ) = 0;
		virtual void notifyCursorIconSetup( const String & _filename ) = 0;

		virtual void notifyCursorClipping( const Viewport & _viewport ) = 0;
		virtual void notifyCursorUnClipping() = 0;

		virtual void notifySoundInitialize() = 0;

		virtual void setAsScreensaver( bool _set ) = 0;
	};

	class ApplicationInterface
	{
	public:
		virtual bool initialize( PlatformInterface* _interface, const String & _platformName, const String& _args ) = 0;
		virtual bool loadConfig( const String& _configFile ) = 0;

		virtual void setBaseDir( const String& _baseDir ) = 0;

	public:
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:
		virtual void setDesktopResolution( const Resolution & _resolution ) = 0;
		virtual void setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight ) = 0;

	public:
		virtual bool getHasWindowPanel() const = 0;
		virtual bool getAllowFullscreenSwitchShortcut() const = 0;

	public:
		virtual void setLanguagePack( const ConstString & _packName ) = 0;

	public:
		virtual bool onRender() = 0;
		virtual void onFlush() = 0;
		virtual bool onUpdate() = 0;
		virtual void onTick( float _timing ) = 0;
		virtual void onFocus( bool _focus ) = 0;
		virtual void onClose() = 0;

		virtual void onTurnSound( bool _turn ) = 0;

		virtual void onAppMouseLeave() = 0;
		virtual void onAppMouseEnter() = 0;

		virtual bool onMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown ) = 0;
		virtual bool onMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) = 0;
		virtual bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) = 0;

		virtual void onPaint() = 0;

	public:
		virtual bool initGame( const String & _scriptInitParams ) = 0;
		virtual bool loadGame() = 0;
		virtual bool loadPersonality() = 0;

	public:
		virtual const String & getProjectTitle() const = 0;

	public:
		virtual int getAlreadyRunningPolicy() const = 0;

	public:
		virtual void setFullscreenMode( bool _fullscreen ) = 0;
		virtual bool getFullscreenMode() const = 0;

	public:
		virtual const Resolution& getResolution() const = 0;

	public:
		virtual bool createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle ) = 0;

	public:
		virtual bool getVSync() const = 0;
		virtual bool isFocus() const = 0;

	public:
		virtual void finalize() = 0;

	public:
		virtual void pushKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) = 0;
		virtual void pushMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown ) = 0;
		virtual void pushMouseMoveEvent( const mt::vec2f & _point, int _x, int _y, int _z ) = 0;

	public:
		virtual void setCursorPosition( const mt::vec2f & _point ) = 0;

		virtual const String& getScreensaverName() const = 0;
	};
	
	typedef void* (*TDynamicLibraryFunction)(ApplicationInterface * _interface);

	class DynamicLibraryInterface
    {
    public:
        virtual void load() = 0;
        virtual void unload() = 0;
		virtual const String& getName() const = 0;
        virtual TDynamicLibraryFunction getSymbol( const String& _name ) const = 0;
    };
}

bool initInterfaceSystem( Menge::ApplicationInterface**	_interface );
void releaseInterfaceSystem( Menge::ApplicationInterface* _interface );
