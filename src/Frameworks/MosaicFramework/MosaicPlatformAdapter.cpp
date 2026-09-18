#include "MosaicPlatformAdapter.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const FileGroupInterfacePtr & getUserFileGroup_()
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            return fileGroup;
        }
        //////////////////////////////////////////////////////////////////////////
        static FilePath makeMosaicFilePath_( Mosaic::StringView _path )
        {
            PathString path;
            path.append( _path.data(), _path.size() );

            FilePath filePath = Helper::stringizeFilePath( path.c_str() );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * getCursorIconName_( Mosaic::CursorShape _cursor )
        {
            switch( _cursor )
            {
            case Mosaic::CursorShape::Arrow:
                return "Arrow";
            case Mosaic::CursorShape::Hand:
                return "Hand";
            case Mosaic::CursorShape::Text:
                return "Text";
            case Mosaic::CursorShape::ResizeHorizontal:
                return "ResizeHorizontal";
            case Mosaic::CursorShape::ResizeVertical:
                return "ResizeVertical";
            case Mosaic::CursorShape::ResizeDiagonalNesw:
                return "ResizeDiagonalNesw";
            case Mosaic::CursorShape::ResizeDiagonalNwse:
                return "ResizeDiagonalNwse";
            case Mosaic::CursorShape::ResizeAll:
                return "ResizeAll";
            case Mosaic::CursorShape::Crosshair:
                return "Crosshair";
            case Mosaic::CursorShape::Wait:
                return "Wait";
            case Mosaic::CursorShape::Progress:
                return "Progress";
            case Mosaic::CursorShape::NotAllowed:
                return "NotAllowed";
            }

            return "Arrow";
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicPlatformAdapter::MosaicPlatformAdapter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicPlatformAdapter::~MosaicPlatformAdapter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicPlatformAdapter::getClipboardText( Mosaic::String * const _out )
    {
        Char value[MENGINE_MAX_PATH + 1] = {'\0'};

        if( PLATFORM_SERVICE()
            ->getClipboardText( value, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        _out->assign( value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::setClipboardText( Mosaic::StringView _text )
    {
        PathString value;
        value.append( _text.data(), _text.size() );

        PLATFORM_SERVICE()
            ->setClipboardText( value.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicPlatformAdapter::writeConsole( Mosaic::StringView _text )
    {
        LOGGER_MESSAGE( "%.*s"
            , (int32_t)_text.size()
            , _text.data()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicPlatformAdapter::readFile( Mosaic::StringView _path, Mosaic::ByteVector * const _out )
    {
        const FileGroupInterfacePtr & fileGroup = Detail::getUserFileGroup_();

        if( fileGroup == nullptr )
        {
            return false;
        }

        FilePath filePath = Detail::makeMosaicFilePath_( _path );

        if( fileGroup->existFile( filePath, false ) == false )
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr )
        {
            return false;
        }

        const std::byte * buffer = memory->getBuffer();
        size_t size = memory->getSize();

        _out->assign( buffer, buffer + size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicPlatformAdapter::writeFile( Mosaic::StringView _path, Mosaic::ByteSpan _data )
    {
        const FileGroupInterfacePtr & fileGroup = Detail::getUserFileGroup_();

        if( fileGroup == nullptr )
        {
            return false;
        }

        FilePath filePath = Detail::makeMosaicFilePath_( _path );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            return false;
        }

        bool successful_write = true;

        if( _data.empty() == false )
        {
            size_t data_size = _data.size();

            successful_write = stream->write( _data.data(), data_size ) == data_size;
        }

        bool successful_close = Helper::closeOutputStreamFile( fileGroup, stream, successful_write );

        if( successful_write == false )
        {
            return false;
        }

        return successful_close;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicPlatformAdapter::userDataPath( Mosaic::StringView _application, Mosaic::StringView _filename, Mosaic::String * const _out )
    {
        MENGINE_UNUSED( _application );

        Char userPath[MENGINE_MAX_PATH + 1] = {'\0'};
        PLATFORM_SERVICE()
            ->getUserPath( userPath );

        _out->assign( userPath );
        _out->append( _filename.data(), _filename.size() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    double MosaicPlatformAdapter::monotonicTime() const noexcept
    {
        double elapsed = TIME_SYSTEM()
            ->getElapsedTime();

        return elapsed * 0.001;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::setCursor( Mosaic::CursorShape _cursor )
    {
        const Char * iconName = Detail::getCursorIconName_( _cursor );

        ConstString icon = Helper::stringizeString( iconName );

        if( PLATFORM_SERVICE()
            ->hasCursorIcon( icon ) == false )
        {
            return;
        }

        PLATFORM_SERVICE()
            ->setCursorIcon( icon );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::setImeCandidateRect( const Mosaic::Rect & _screenRect )
    {
        MENGINE_UNUSED( _screenRect );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::openUrl( Mosaic::StringView _url )
    {
        PathString url;
        url.append( _url.data(), _url.size() );

        PLATFORM_SERVICE()
            ->openUrlInDefaultBrowser( url.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    void * MosaicPlatformAdapter::createWindow( const Mosaic::NativeWindowDescription & _description )
    {
        MENGINE_UNUSED( _description );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::destroyWindow( void * _nativeHandle )
    {
        MENGINE_UNUSED( _nativeHandle );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::showWindow( void * _nativeHandle, bool _visible )
    {
        MENGINE_UNUSED( _nativeHandle );
        MENGINE_UNUSED( _visible );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::setWindowBounds( void * _nativeHandle, const Mosaic::Rect & _bounds )
    {
        MENGINE_UNUSED( _nativeHandle );
        MENGINE_UNUSED( _bounds );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::MonitorSpan MosaicPlatformAdapter::monitors() const noexcept
    {
        Resolution desktopResolution;
        PLATFORM_SERVICE()
            ->getDesktopResolution( &desktopResolution );

        Mosaic::Monitor monitor;
        monitor.id = 1;
        monitor.bounds = {0.f, 0.f, (float)desktopResolution.getWidth(), (float)desktopResolution.getHeight()};
        monitor.workArea = monitor.bounds;
        monitor.dpiScale = 1.f;
        monitor.primary = true;

        m_monitors.assign( 1, monitor );

        const Mosaic::Monitor * monitorData = m_monitors.data();
        size_t monitorCount = m_monitors.size();

        Mosaic::MonitorSpan monitors( monitorData, monitorCount );

        return monitors;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicPlatformAdapter::publishAccessibilityTree( Mosaic::SemanticNodeSpan _semantics )
    {
        MENGINE_UNUSED( _semantics );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
