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
#	include "Interface/GameInterface.h"

namespace Menge
{
	class ApplicationInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ApplicationService");

	public:
		virtual bool initialize( const String & _args ) = 0;
        virtual void finalize() = 0;
		
	public:
		virtual void finalizeGame() = 0;
			
	public:
		virtual bool getAllowFullscreenSwitchShortcut() const = 0;

	public:
		virtual bool render() = 0;
		virtual void flush() = 0;
		virtual bool beginUpdate() = 0;
		virtual void tick( float _timing ) = 0;
		virtual void endUpdate() = 0;
		virtual void setFocus( bool _focus, bool _mouse, const mt::vec2f & _point ) = 0;
		virtual void close() = 0;

		virtual void turnSound( bool _turn ) = 0;


	public:
		virtual bool keyEvent( const InputKeyEvent & _event ) = 0;

		virtual bool mouseButtonEvent( const InputMouseButtonEvent& _event ) = 0;
		virtual bool mouseMove( const InputMouseMoveEvent& _event ) = 0;
		virtual bool mouseWheel( const InputMouseWheelEvent & _event ) = 0;
		virtual void mousePosition( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseLeave( const InputMousePositionEvent & _event ) = 0;
        
	public:
		virtual void paint() = 0;
        virtual bool userEvent( const ConstString & _event, const TMapParams & _params ) = 0;

	public:
		virtual void setDefaultWindowDescription( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _vsync ) = 0;

	public:
		virtual bool loadResourcePacks( const ConstString & _fileGroup, const FilePath & _resourceIni ) = 0;
		virtual bool initializeGame( const ConstString & _module, const ConstString & _language, const FilePath & _accountPath, const String & _scriptInitParams ) = 0;
        
	public:
		virtual const WString & getCompanyName() const = 0;
		virtual const WString & getProjectName() const = 0;

	public:
		virtual const ConstString & getProjectTitle() const = 0;
		virtual const ConstString & getProjectCodename() const = 0;
		virtual uint32_t getProjectVersion() const = 0;

	public:
		virtual void changeWindowResolution( const Resolution & _resolution ) = 0;

	public:
		virtual void setFullscreenMode( bool _fullscreen ) = 0;
		virtual bool getFullscreenMode() const = 0;

    public:
        virtual void setFixedContentResolution( bool _fixedContetResolution ) = 0; 
        virtual bool getFixedContentResolution() const = 0;

    public:
        virtual bool isValidWindowMode() const = 0;
		virtual bool isDevelopmentMode() const = 0;

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

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

	public:
		virtual void debugPause( bool _pause ) = 0;

    public:
        virtual void quit() = 0;
	};

#   define APPLICATION_SERVICE( serviceProvider )\
    ((Menge::ApplicationInterface*)SERVICE_GET(serviceProvider, Menge::ApplicationInterface))
}