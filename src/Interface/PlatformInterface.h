#   pragma once

#	include "Config/Typedef.h"
#   include "Config/String.h"

#	include "Core/ConstString.h"
#	include "Core/Tags.h"
#   include "Core/FilePath.h"
#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/Params.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

namespace Menge
{
    class PlatformInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PlatformService");
		
	public:
		virtual void update() = 0;
		virtual void stop() = 0;

    public:
        virtual void setIcon( uint32_t _icon ) = 0;
        virtual uint32_t getIcon() const = 0;

        virtual void setProjectTitle( const WString & _projectTitle ) = 0;
        virtual const WString & getProjectTitle() const = 0;

	public:
		virtual bool createWindow( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual Pointer getWindowHandle() const = 0;

	public:
		virtual const Tags & getPlatformTags() const = 0;

	public:
		virtual bool hasTouchpad() const = 0;

    public:
        virtual bool getDesktopResolution( Resolution & _resolution ) const = 0;

		virtual size_t getCurrentPath( WChar * _path, size_t _len ) const = 0;
		virtual size_t getUserPath( WChar * _path, size_t _len ) const = 0;

        virtual void minimizeWindow() = 0;

		virtual void setCursorPosition( const mt::vec2f & _pos ) = 0;

        virtual void showKeyboard() = 0;
        virtual void hideKeyboard() = 0;

        virtual void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void notifyVsyncChanged( bool _vsync ) = 0;
        virtual void notifyCursorModeChanged( bool _mode ) = 0;
		virtual bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) = 0;

    public:
        virtual void onEvent( const ConstString & _event, const TMapWParams & _params ) = 0;

    public:
		virtual size_t getShortPathName( const WString & _path, WChar * _short, size_t _len ) const = 0;

    public:
        virtual void getMaxClientResolution( Resolution & _resolution ) const = 0;

    public:
        virtual bool openUrlInDefaultBrowser( const WString & _url ) = 0;       

	public:
		virtual bool existDirectory( const WString & _path ) const = 0;
		virtual bool createDirectory( const WString & _path ) = 0;

	public:
		virtual bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) = 0;
		virtual bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) = 0;

	public:
		virtual bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity ) = 0;
    };

#   define PLATFORM_SERVICE( serviceProvider )\
    ((Menge::PlatformInterface*)SERVICE_GET(serviceProvider, Menge::PlatformInterface))
}