#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorFilePath.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/AspectRatioViewport.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Config/UniqueId.h"
#include "Config/Timestamp.h"

#ifndef MENGINE_APPLICATION_COMPANY_MAXNAME
#define MENGINE_APPLICATION_COMPANY_MAXNAME 256
#endif

#ifndef MENGINE_APPLICATION_PROJECT_MAXNAME
#define MENGINE_APPLICATION_PROJECT_MAXNAME 256
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EContentEdgeMode
    {
        ECEM_NONE,
        ECEM_HORIZONTAL_CONTENT_EDGE,
        ECEM_VERTICAL_CONTENT_EDGE
    };
    //////////////////////////////////////////////////////////////////////////
    class ApplicationInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "Application" );

    public:
        virtual bool getAllowFullscreenSwitchShortcut() const = 0;

    public:
        virtual bool render() = 0;
        virtual void flush() = 0;
        virtual bool beginUpdate() = 0;
        virtual void tick( float _time ) = 0;
        virtual void endUpdate() = 0;
        virtual void setFocus( bool _focus ) = 0;
        virtual void setFreeze( bool _freeze ) = 0;
        virtual void close() = 0;

        virtual void turnSound( bool _turn ) = 0;

    public:
        virtual bool initializeGame( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _resourcePaths ) = 0;
        virtual void finalizeGame() = 0;

    public:
        virtual size_t getCompanyName( Char * const _companyName ) const = 0;
        virtual size_t getProjectName( Char * const _projectName ) const = 0;

    public:
        virtual const ConstString & getProjectCodename() const = 0;
        virtual uint32_t getProjectVersion() const = 0;

    public:
        virtual void setLocale( const ConstString & _locale ) = 0;
        virtual const ConstString & getLocale() const = 0;

    public:
        virtual void setWindowResolution( const Resolution & _resolution ) = 0;
        virtual const Resolution & getWindowResolution() const = 0;

    public:
        virtual void setFullscreenMode( bool _fullscreen ) = 0;
        virtual bool getFullscreenMode() const = 0;

        virtual uint32_t getWindowBits() const = 0;
        virtual int32_t getWindowFSAAType() const = 0;
        virtual int32_t getWindowFSAAQuality() const = 0;

    public:
        virtual void setFixedContentResolution( bool _fixedContetResolution ) = 0;
        virtual bool getFixedContentResolution() const = 0;

        virtual void setFixedDisplayResolution( bool _fixedDisplayResolution ) = 0;
        virtual bool getFixedDisplayResolution() const = 0;

        virtual void setFixedViewportResolution( bool _fixedViewportResolution ) = 0;
        virtual bool getFixedViewportResolution() const = 0;

    public:
        virtual bool calcWindowResolution( bool _fullscreen, Resolution * const _windowResolution ) const = 0;

        virtual const Resolution & getCurrentWindowResolution() const = 0;
        virtual const Viewport & getRenderViewport() const = 0;

        virtual const Resolution & getContentResolution() const = 0;
        virtual void getGameViewport( float * const _aspect, Viewport * const _viewport ) const = 0;

        virtual EContentEdgeMode getAspectRatioContentEdgeMode() const = 0;

    public:
        virtual bool createRenderWindow() = 0;

    public:
        virtual void setVSync( bool _vsync ) = 0;
        virtual bool getVSync() const = 0;

        virtual bool isFocus() const = 0;
        virtual bool isFrozen() const = 0;

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

        virtual void setVideoEnable( bool _enabled ) = 0;
        virtual bool getVideoEnable() const = 0;

    public:
        virtual uint32_t getDebugMask() const = 0;

    public:
        virtual void debugPause( bool _pause ) = 0;

    public:
        virtual void quit() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define APPLICATION_SERVICE()\
    ((Mengine::ApplicationInterface *)SERVICE_GET(Mengine::ApplicationInterface))
//////////////////////////////////////////////////////////////////////////