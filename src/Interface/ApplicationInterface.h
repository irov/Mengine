#	pragma once

#	include "Config/Typedef.h"
#   include "Config/String.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"
#   include "Core/AspectRatioViewports.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/ModuleInterface.h"

namespace Menge
{
	struct ApplicationSettings
	{
		ApplicationSettings()
			: projectVersion(0)
			, projectVersionCheck(true)
			, bits(32)
			, fullscreen(true)
			, vsync(true)
			, windowModeCheck(false)
			, fixedContentResolution(true)
		{
		}

		FilePath baseDir;

		ConstString platformName;
		ConstString projectCodename;
		size_t projectVersion;
		bool projectVersionCheck;

		Resolution contentResolution;
        Resolution windowResolution;

		TVectorAspectRatioViewports aspectRatioViewports;
		
		size_t bits;
		bool fullscreen;
		bool vsync;        

        bool windowModeCheck;
        bool fixedContentResolution;
	};

	struct ResourcePackDesc
	{
        ConstString name;
		ConstString type;

		ConstString locale;
		ConstString platform;

        FilePath path;
		FilePath descriptionPath;

        bool dev;
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

		virtual const FilePath & getBaseDir() const = 0;
		
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

        virtual bool onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) = 0;
        virtual bool onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown ) = 0;
        virtual bool onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) = 0;
		virtual bool onMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _whell ) = 0;
		virtual void onMousePosition( unsigned int _touchId, const mt::vec2f & _point ) = 0;
        
		virtual void onPaint() = 0;
        virtual bool onUserEvent( const ConstString & _event, const TMapParams & _params ) = 0;

	public:		
		virtual bool createGame( const WString & _module, const ConstString & _language, const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack ) = 0;
		virtual bool initializeGame( const TMapParams & _params, const String & _scriptInitParams ) = 0;
        
	public:
		virtual const ConstString & getProjectTitle() const = 0;
		virtual const ConstString & getProjectCodename() const = 0;
		virtual size_t getProjectVersion() const = 0;

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

        virtual const ConstString & getPlatformName() const = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

    public:
        virtual void quit() = 0;
	};

#   define APPLICATION_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ApplicationInterface)
}