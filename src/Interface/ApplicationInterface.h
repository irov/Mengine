#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Interface/ServiceInterface.h"
#   include "Interface/WatchdogInterface.h"

namespace Menge
{
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
        : public ServiceInterface
	{
        SERVICE_DECLARE("PlatformService");

	public:
		virtual void stop() = 0;

		virtual void getDesktopResolution( Resolution & _resolution ) const = 0;

		virtual const String & getCurrentPath() const = 0;

		virtual void minimizeWindow() = 0;

		virtual void setHandleMouse( bool _handle ) = 0;
		virtual void setCursorPosition( int _x, int _y ) = 0;

		virtual TimerInterface * getTimer() const = 0;

		virtual void showKeyboard() = 0;
		virtual void hideKeyboard() = 0;

		virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void notifyVsyncChanged( bool _vsync ) = 0;
		virtual void notifyCursorModeChanged( bool _mode ) = 0;
		virtual void notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size ) = 0;

		virtual void notifyCursorClipping( const Viewport & _viewport ) = 0;
		virtual void notifyCursorUnClipping() = 0;

    public:
        virtual size_t getShortPathName( const String & _name, char * _shortpath, size_t _shortpathlen ) = 0;

    public:
        virtual void getMaxClientResolution( Resolution & _resolution ) const = 0;

    public:
        virtual bool isDevelopmentMode() const = 0;
		//virtual void notifySoundInitialize() = 0;

		//virtual void setAsScreensaver( bool _set ) = 0;

	public:
		virtual bool openUrlInDefaultBrowser( const WString & _url ) = 0;
	};

#   define PLATFORM_SERVICE( serviceProvider )\
    (Menge::getService<Menge::PlatformInterface>(serviceProvider))

    struct AspectRatioViewports
    {
        float width;
        float height;

        Viewport viewport;
    };

    typedef std::vector<AspectRatioViewports> TVectorAspectRatioViewports;

	struct ApplicationSettings
	{
		String baseDir;

		String platformName;        		
		String projectCodename;

		Resolution contentResolution;

		bool fixedContentResolution;

		Resolution windowResolution;
		size_t bits;
		bool fullscreen;
		bool vsync;		

        bool windowModeCheck;

        
        TVectorAspectRatioViewports aspectRatioViewports;
	};

	struct ResourcePackDesc
	{
        String name;
		String type;

		String locale;
		String platform;

        String path;
		String description;

		bool preload;
	};

	typedef std::vector<ResourcePackDesc> TVectorResourcePackDesc;

	class ApplicationInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ApplicationService");

	public:
		virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool setup( const String& _args, const ApplicationSettings & _setting ) = 0;

		virtual const String & getBaseDir() const = 0;
		
	public:
		virtual PlatformInterface * getPlatform() const = 0;

	public:
		virtual bool getAllowFullscreenSwitchShortcut() const = 0;

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

        virtual bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) = 0;
        virtual bool onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown ) = 0;
        virtual bool onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) = 0;
        
		virtual void onPaint() = 0;

	public:		
		virtual bool createGame( const WString & _module, const ConstString & _language, const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack ) = 0;
		virtual bool initializeGame( const TMapWString & _params, const String & _scriptInitParams ) = 0;
        
	public:
		virtual const WString & getProjectTitle() const = 0;
		virtual const String & getProjectCodename() const = 0;

	public:
		virtual void setFullscreenMode( bool _fullscreen ) = 0;
		virtual bool getFullscreenMode() const = 0;

    public:
        virtual void setFixedContentResolution( bool _fixedContetResolution ) = 0; 
        virtual bool getFixedContentResolution() const = 0;

    public:
        virtual bool isValidWindowMode() const = 0;

	public:
		virtual void calcWindowResolution( Resolution & _windowResolution ) const = 0;

		virtual const Resolution & getCurrentResolution() const = 0;
		virtual const Viewport & getRenderViewport() const = 0;
		
		virtual const Resolution & getContentResolution() const = 0;
		virtual void getGameViewport( float & _aspect, Viewport & _viewport ) const = 0;

	public:
		virtual bool createRenderWindow( WindowHandle _renderWindowHandle ) = 0;

	public:
        virtual void setVSync( bool _vsync ) = 0;
		virtual bool getVSync() const = 0;

		virtual bool isFocus() const = 0;

    public:
        virtual void setInputMouseButtonEventBlock( bool _block ) = 0;
        virtual bool getInputMouseButtonEventBlock() const = 0;


    public:
        virtual void setCursorMode( bool _mode ) = 0;
        virtual bool getCursorMode() const = 0;
        virtual void setCursorIcon( const ConstString & _resourceName ) = 0;

    public:
        virtual void minimizeWindow() = 0;

    public:
        virtual void setParticlesEnabled( bool _enabled ) = 0;
        virtual bool getParticlesEnabled() const = 0;

        virtual void setMouseBounded( bool _bounded ) = 0;
        virtual bool getMouseBounded() const = 0;

        virtual unsigned int getDebugMask() const = 0;

        virtual const String & getPlatformName() const = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

    public:
        virtual WatchdogInterface * getWatchdog() const = 0;

    public:
        virtual void quit() = 0;

	public:		
		virtual void destroy() = 0;
	};

#   define APPLICATION_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ApplicationInterface>(serviceProvider))
}