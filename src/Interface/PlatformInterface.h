#pragma once

#include "Config/Typedef.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"
#include "Kernel/Params.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DynamicLibraryInterface.h"

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
        virtual void update() = 0;
        virtual void stopPlatform() = 0;

    public:
        virtual void setIcon( uint32_t _icon ) = 0;
        virtual uint32_t getIcon() const = 0;

        virtual void setProjectTitle( const Char * _projectTitle ) = 0;
        virtual size_t getProjectTitle( Char * _projectTitle ) const = 0;

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

        virtual size_t getCurrentPath( Char * _path ) const = 0;
        virtual size_t getUserPath( Char * _path ) const = 0;

        virtual void minimizeWindow() = 0;

        virtual void setCursorPosition( const mt::vec2f & _pos ) = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

        virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void notifyVsyncChanged( bool _vsync ) = 0;
        virtual void notifyCursorModeChanged( bool _mode ) = 0;
        virtual bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) = 0;

    public:
        virtual void onEvent( const ConstString & _event, const MapWParams & _params ) = 0;

    public:
        virtual float getJoystickAxis( uint32_t _index ) const = 0;

    public:
        virtual size_t getShortPathName( const Char * _path, Char * _short, size_t _len ) const = 0;

    public:
        virtual void getMaxClientResolution( Resolution & _resolution ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const Char * _url ) = 0;

    public:
        virtual bool existDirectory( const Char * _path ) const = 0;
        virtual bool createDirectory( const Char * _path ) = 0;

    public:
        virtual bool existFile( const Char * _path ) = 0;
        virtual bool removeFile( const Char * _path ) = 0;

    public:
        virtual uint64_t getFileTime( const Char * _path ) const = 0;

    public:
        virtual bool createDirectoryUserPicture( const Char * _path, const Char * _file, const void * _data, size_t _size ) = 0;
        virtual bool createDirectoryUserMusic( const Char * _path, const Char * _file, const void * _data, size_t _size ) = 0;

    public:
        virtual bool getErrorMessage( uint32_t _messageId, Char * _out ) const = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual bool cmd( const Char * _command ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLATFORM_SERVICE()\
    ((Mengine::PlatformInterface*)SERVICE_GET(Mengine::PlatformInterface))
//////////////////////////////////////////////////////////////////////////