#pragma once

#include "Config/Typedef.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Params.h"
#include "Kernel/LambdaFilePath.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DynamicLibraryInterface.h"
#include "Interface/DateTimeProviderInterface.h"
#include "Interface/ContentInterface.h"

#ifndef MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME
#define MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME 256
#endif

namespace Mengine
{
    class PlatformInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PlatformService" );

    public:
        virtual bool runPlatform() = 0;
        virtual void updatePlatform() = 0;
        virtual void stopPlatform() = 0;

    public:
        typedef Lambda<void()> LambdaTimer;
        virtual uint32_t addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc ) = 0;
        virtual void removeTimer( uint32_t _id ) = 0;

    public:
        virtual uint64_t getTicks() const = 0;

    public:
        virtual void setIcon( uint32_t _icon ) = 0;
        virtual uint32_t getIcon() const = 0;

        virtual void setProjectTitle( const Char * _projectTitle ) = 0;
        virtual void getProjectTitle( Char * _projectTitle ) const = 0;

    public:
        virtual bool createWindow( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual Pointer getWindowHandle() const = 0;

    public:
        virtual const Tags & getPlatformTags() const = 0;
        virtual bool hasPlatformTag( const ConstString & _tag ) const = 0;

    public:
        virtual bool hasTouchpad() const = 0;

    public:
        virtual DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName ) = 0;

    public:
        virtual bool getDesktopResolution( Resolution & _resolution ) const = 0;

        virtual bool getNoFullscreen() const = 0;
        virtual bool getAlwaysFullscreen() const = 0;

        virtual bool setProcessDPIAware() = 0;

        virtual bool isDebuggerPresent() const = 0;
        virtual bool createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full ) = 0;

        virtual size_t getCurrentPath( Char * _filePath ) const = 0;
        virtual size_t getUserPath( Char * _filePath ) const = 0;

        virtual void minimizeWindow() = 0;

        virtual void setCursorPosition( const mt::vec2f & _pos ) = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

        virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void notifyVsyncChanged( bool _vsync ) = 0;
        virtual void notifyCursorModeChanged( bool _mode ) = 0;
        virtual bool notifyCursorIconSetup( const ConstString & _name, const ContentInterface * _content, const MemoryInterfacePtr & _memory ) = 0;

    public:
        virtual void onEvent( const ConstString & _event, const MapWParams & _params ) = 0;

    public:
        virtual float getJoystickAxis( uint32_t _index ) const = 0;

    public:
        virtual size_t getShortPathName( const Char * _filePath, Char * _shortFilePath, size_t _len ) const = 0;
        virtual size_t getSystemFontPath( const Char * _fontName, Char * _fontPath ) const = 0;

    public:
        virtual void getMaxClientResolution( Resolution & _resolution ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;

    public:
        virtual bool existDirectory( const Char * _directoryPath ) const = 0;
        virtual bool createDirectory( const Char * _directoryPath ) = 0;

    public:
        virtual bool existFile( const Char * _filePath ) = 0;
        virtual bool removeFile( const Char * _filePath ) = 0;

    public:
        virtual bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const = 0;

    public:
        virtual uint64_t getFileTime( const Char * _filePath ) const = 0;

    public:
        virtual DateTimeProviderInterfacePtr createDateTimeProvider( const DocumentPtr & _doc ) = 0;

    public:
        virtual bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) = 0;

    public:
        virtual bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;
        virtual bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) = 0;

    public:
        virtual bool getErrorMessage( uint32_t _messageId, Char * _out, size_t _capacity ) const = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual bool getLocalMachineRegValue( const Char * _path, const Char * _key, Char * _value, size_t _size ) = 0;

    public:
        virtual bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * _exitCode ) = 0;

    public:
        virtual void abort() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SERVICE()\
    ((Mengine::PlatformInterface*)SERVICE_GET(Mengine::PlatformInterface))
//////////////////////////////////////////////////////////////////////////