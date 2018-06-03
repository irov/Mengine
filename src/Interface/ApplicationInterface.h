#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"
#include "Core/Resolution.h"
#include "Core/Viewport.h"
#include "Core/AspectRatioViewports.h"

#include "Interface/ServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/GameInterface.h"
#include "Interface/FileSystemInterface.h"

namespace Mengine
{
	class ApplicationInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ApplicationService");
			
	public:
		virtual bool getAllowFullscreenSwitchShortcut() const = 0;

	public:
		virtual bool render() = 0;
		virtual void flush() = 0;
		virtual bool beginUpdate() = 0;
		virtual void tick( float _timing ) = 0;
		virtual void endUpdate() = 0;
		virtual void setFocus( bool _focus ) = 0;
		virtual void setFreeze( bool _freeze ) = 0;
		virtual void close() = 0;

		virtual void turnSound( bool _turn ) = 0;
		
	public:
		virtual bool keyEvent( const InputKeyEvent & _event ) = 0;
		virtual bool textEvent( const InputTextEvent & _event ) = 0;

		virtual bool mouseButtonEvent( const InputMouseButtonEvent& _event ) = 0;
		virtual bool mouseMove( const InputMouseMoveEvent& _event ) = 0;
		virtual bool mouseWheel( const InputMouseWheelEvent & _event ) = 0;
		virtual void mousePosition( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseLeave( const InputMousePositionEvent & _event ) = 0;
        
	public:
		virtual void paint() = 0;

	public:
		virtual bool initializeGame( const FileGroupInterfacePtr & _category, const FilePath & _resourceIniPath ) = 0;
        
	public:
		virtual const WString & getCompanyName() const = 0;
		virtual const WString & getProjectName() const = 0;

	public:
		virtual const ConstString & getProjectCodename() const = 0;
		virtual uint32_t getProjectVersion() const = 0;
		
	public:
	public:
		virtual void setLocale( const ConstString & _locale ) = 0;
		virtual const ConstString & getLocale() const = 0;

	public:
		virtual void changeWindowResolution( const Resolution & _resolution ) = 0;

	public:
		virtual void setFullscreenMode( bool _fullscreen ) = 0;
		virtual bool getFullscreenMode() const = 0;

        virtual uint32_t getWindowBits() const = 0;
        virtual int getWindowFSAAType() const = 0;
        virtual int getWindowFSAAQuality() const = 0;

    public:
        virtual void setFixedContentResolution( bool _fixedContetResolution ) = 0; 
        virtual bool getFixedContentResolution() const = 0;

		virtual void setFixedDisplayResolution( bool _fixedDisplayResolution ) = 0;
		virtual bool getFixedDisplayResolution() = 0;


    public:
        virtual bool isValidWindowMode() const = 0;

	public:
		virtual void calcWindowResolution( Resolution & _windowResolution ) const = 0;

		virtual const Resolution & getCurrentResolution() const = 0;
		virtual const Viewport & getRenderViewport() const = 0;
		
		virtual const Resolution & getContentResolution() const = 0;
		virtual void getGameViewport( float & _aspect, Viewport & _viewport ) const = 0;

	public:
		virtual bool createRenderWindow() = 0;

	public:
        virtual void setVSync( bool _vsync ) = 0;
		virtual bool getVSync() const = 0;
		
		virtual bool isFocus() const = 0;

		virtual void setNopause( bool _nopause ) = 0;
		virtual bool getNopause() const = 0;

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
        virtual void setParticleEnable( bool _enable ) = 0;
        virtual bool getParticleEnable() const = 0;

		virtual void setTextEnable( bool _enable ) = 0;
		virtual bool getTextEnable() const = 0;

	public:
        virtual uint32_t getDebugMask() const = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

	public:
		virtual void debugPause( bool _pause ) = 0;

    public:
        virtual void quit() = 0;
	};

#   define APPLICATION_SERVICE()\
    ((Mengine::ApplicationInterface*)SERVICE_GET(Mengine::ApplicationInterface))
}