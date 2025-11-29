#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DynamicLibraryInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/UnknownInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Params.h"
#include "Kernel/UnknownPointer.h"
#include "Kernel/LambdaFilePath.h"
#include "Kernel/Unknowable.h"

#include "Config/UniqueId.h"
#include "Config/Timestamp.h"
#include "Config/ThreadId.h"

#ifndef MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME
#define MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME 256
#endif

namespace Mengine
{
    class PlatformServiceInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "PlatformService" );

    public:
        virtual bool initializeFileService() = 0;
        virtual void finalizeFileService() = 0;

    public:
        virtual bool runPlatform() = 0;
        virtual void loopPlatform() = 0;
        virtual bool updatePlatform() = 0;
        virtual bool tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause ) = 0;
        virtual void stopPlatform() = 0;
        virtual void freezePlatform( bool _tick, bool _render, bool _sound ) = 0;
        virtual void unfreezePlatform( bool _tick, bool _render, bool _sound ) = 0;


    public:
        virtual void setSleepMode( bool _sleepMode ) = 0;
        virtual bool getSleepMode() const = 0;

    public:
        virtual Timestamp getPlatfomTime() const = 0;

    public:
        virtual void setProjectTitle( const Char * _projectTitle ) = 0;
        virtual size_t getProjectTitle( Char * const _projectTitle ) const = 0;

    public:
        virtual bool alreadyRunningMonitor() = 0;

    public:
        virtual bool createWindow( const Resolution & _resolution, bool _fullscreen ) = 0;

    public:
        virtual const Tags & getPlatformTags() const = 0;
        virtual bool hasPlatformTag( const ConstString & _tag ) const = 0;

    public:
        virtual bool isDesktop() const = 0;
        virtual bool hasTouchpad() const = 0;

    public:
        virtual int32_t dpToWidthPx( int32_t _dp ) const = 0;
        virtual int32_t dpToHeightPx( int32_t _dp ) const = 0;

        virtual int32_t pxToWidthDp( int32_t _px ) const = 0;
        virtual int32_t pxToHeightDp( int32_t _px ) const = 0;

    public:
        virtual DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool getDesktopResolution( Resolution * const _resolution ) const = 0;

        virtual bool getNoFullscreen() const = 0;
        virtual bool getAlwaysFullscreen() const = 0;

        virtual bool isDebuggerPresent() const = 0;
        virtual void debugBreak() = 0;

        virtual size_t getCurrentPath( Char * const _filePath ) const = 0;
        virtual size_t getUserPath( Char * const _filePath ) const = 0;
        virtual size_t getExtraPreferencesFolderName( Char * const _folderName ) const = 0;        
        virtual bool getUserLocaleLanguage( Char * const _userLocaleLanguage ) const = 0;

        virtual void closeWindow() = 0;
        virtual void minimizeWindow() = 0;

        virtual void setCursorPosition( const mt::vec2f & _cursorPosition ) = 0;
        virtual void setCursorIcon( const ConstString & _icon ) = 0;
        virtual bool hasCursorIcon( const ConstString & _icon ) const = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;
        virtual bool isShowKeyboard() const = 0;

    public:
        virtual bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void notifyVsyncChanged( bool _vsync ) = 0;
        virtual void notifyCursorModeChanged( bool _mode ) = 0;
        virtual bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _memory ) = 0;

    public:
        virtual void onEvent( const ConstString & _event, const Params & _params ) = 0;

    public:
        virtual size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const = 0;

    public:
        virtual bool getMaxClientResolution( Resolution * const _resolution ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;
        virtual bool openDeleteAccount() = 0;

    public:
        virtual bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) = 0;

    public:
        virtual bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;
        virtual bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;

    public:
        virtual void messageBox( const Char * _caption, const Char * _format, ... ) const = 0;

    public:
        virtual bool setClipboardText( const Char * _value ) const = 0;
        virtual bool getClipboardText( Char * _value, size_t _capacity ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SERVICE()\
    ((Mengine::PlatformServiceInterface *)SERVICE_GET(Mengine::PlatformServiceInterface))
//////////////////////////////////////////////////////////////////////////
