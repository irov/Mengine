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

#ifndef MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME
#define MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME 256
#endif

#ifndef MENGINE_PLATFORM_USER_MAXNAME
#define MENGINE_PLATFORM_USER_MAXNAME 256
#endif

#ifndef MENGINE_PLATFORM_FINGERPRINT_MAXNAME
#define MENGINE_PLATFORM_FINGERPRINT_MAXNAME 41
#endif

#ifndef MENGINE_PLATFORM_DEVICE_MODEL_MAXNAME
#define MENGINE_PLATFORM_DEVICE_MODEL_MAXNAME 128
#endif

#ifndef MENGINE_PLATFORM_DEVICE_LANGUAGE_MAXNAME
#define MENGINE_PLATFORM_DEVICE_LANGUAGE_MAXNAME 64
#endif

#ifndef MENGINE_PLATFORM_OS_FAMILY_MAXNAME
#define MENGINE_PLATFORM_OS_FAMILY_MAXNAME 128
#endif

#ifndef MENGINE_PLATFORM_OS_VERSION_MAXNAME
#define MENGINE_PLATFORM_OS_VERSION_MAXNAME 128
#endif

#ifndef MENGINE_PLATFORM_BUNDLEID_MAXNAME
#define MENGINE_PLATFORM_BUNDLEID_MAXNAME 256
#endif

#ifndef MENGINE_PLATFORM_SESSIONID_MAXNAME
#define MENGINE_PLATFORM_SESSIONID_MAXNAME 128
#endif

#ifndef MENGINE_PLATFORM_INSTALLKEY_MAXNAME
#define MENGINE_PLATFORM_INSTALLKEY_MAXNAME 128
#endif

#ifndef MENGINE_PLATFORM_INSTALLVERSION_MAXNAME
#define MENGINE_PLATFORM_INSTALLVERSION_MAXNAME 64
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
        virtual bool tickPlatform( float _time, bool _render, bool _flush, bool _pause ) = 0;
        virtual void stopPlatform() = 0;

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
        virtual DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool getDesktopResolution( Resolution * const _resolution ) const = 0;

        virtual bool getNoFullscreen() const = 0;
        virtual bool getAlwaysFullscreen() const = 0;

        virtual bool setProcessDPIAware() = 0;

        virtual bool isDebuggerPresent() const = 0;
        virtual void debugBreak() = 0;

        virtual size_t getCurrentPath( Char * const _filePath ) const = 0;
        virtual size_t getUserPath( Char * const _filePath ) const = 0;
        virtual size_t getExtraPreferencesFolderName( Char * const _folderName ) const = 0;
        virtual size_t getUserName( Char * const _userName ) const = 0;
        virtual size_t getDeviceLanguage( Char * const _deviceLanguage ) const = 0;
        virtual size_t getFingerprint( Char * const _fingerprint ) const = 0;

        virtual size_t getDeviceModel( Char * const _deviceModel ) const = 0;
        virtual size_t getOsFamily( Char * const _osFamily ) const = 0;
        virtual size_t getOsVersion( Char * const _osVersion ) const = 0;
        virtual size_t getBundleId( Char * const _osVersion ) const = 0;

        virtual size_t getSessionId( Char * const _sessionId ) const = 0;
        virtual size_t getInstallKey( Char * const _installKey ) const = 0;
        virtual int64_t getInstallTimestamp() const = 0;
        virtual size_t getInstallVersion( Char * const _installVersion ) const = 0;
        virtual int64_t getInstallRND() const = 0;
        virtual int64_t getSessionIndex() const = 0;

        virtual void closeWindow() = 0;
        virtual void minimizeWindow() = 0;

        virtual void setCursorPosition( const mt::vec2f & _pos ) = 0;
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
        virtual void onEvent( const ConstString & _event, const MapWParams & _params ) = 0;

    public:
        virtual float getJoystickAxis( uint32_t _index ) const = 0;

    public:
        virtual size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const = 0;

    public:
        virtual bool getMaxClientResolution( Resolution * const _resolution ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;
        virtual bool openMail( const Char * _email, const Char * _subject, const Char * _body ) = 0;

    public:
        virtual bool existDirectory( const Char * _directoryPath, const Char * _directory ) const = 0;
        virtual bool createDirectory( const Char * _directoryPath, const Char * _directory ) = 0;

    public:
        virtual bool existFile( const Char * _filePath ) = 0;
        virtual bool removeFile( const Char * _filePath ) = 0;
        virtual bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) = 0;

    public:
        virtual bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const = 0;

    public:
        virtual uint64_t getFileTime( const Char * _filePath ) const = 0;

    public:
        virtual bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) = 0;

    public:
        virtual bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;
        virtual bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual void messageBox( const Char * _caption, const Char * _format, ... ) const = 0;

    public:
        virtual bool setClipboardText( const Char * _value ) const = 0;
        virtual bool getClipboardText( Char * _value, size_t _capacity ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SERVICE()\
    ((Mengine::PlatformServiceInterface*)SERVICE_GET(Mengine::PlatformServiceInterface))
//////////////////////////////////////////////////////////////////////////
