#include "ToolMosaicPlatform.h"

#include "Config/Config.h"

#include "GLFW/glfw3.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int getStandardCursorShape_( Mosaic::CursorShape _cursor )
        {
            switch( _cursor )
            {
            case Mosaic::CursorShape::Arrow:
                return GLFW_ARROW_CURSOR;
            case Mosaic::CursorShape::Hand:
                return GLFW_POINTING_HAND_CURSOR;
            case Mosaic::CursorShape::Text:
                return GLFW_IBEAM_CURSOR;
            case Mosaic::CursorShape::ResizeHorizontal:
                return GLFW_RESIZE_EW_CURSOR;
            case Mosaic::CursorShape::ResizeVertical:
                return GLFW_RESIZE_NS_CURSOR;
            case Mosaic::CursorShape::ResizeDiagonalNesw:
                return GLFW_RESIZE_NESW_CURSOR;
            case Mosaic::CursorShape::ResizeDiagonalNwse:
                return GLFW_RESIZE_NWSE_CURSOR;
            case Mosaic::CursorShape::ResizeAll:
                return GLFW_RESIZE_ALL_CURSOR;
            case Mosaic::CursorShape::Crosshair:
                return GLFW_CROSSHAIR_CURSOR;
            case Mosaic::CursorShape::NotAllowed:
                return GLFW_NOT_ALLOWED_CURSOR;
            case Mosaic::CursorShape::Wait:
            case Mosaic::CursorShape::Progress:
                return GLFW_ARROW_CURSOR;
            }

            return GLFW_ARROW_CURSOR;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::String makePath_( Mosaic::StringView _path )
        {
            Mosaic::String path( _path.data(), _path.size() );

            return path;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicPlatform::ToolMosaicPlatform()
        : m_window( nullptr )
        , m_cursors()
        , m_cursor( Mosaic::CursorShape::Arrow )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicPlatform::~ToolMosaicPlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::initialize( GLFWwindow * _window )
    {
        m_window = _window;

        m_cursors.fill( nullptr );

        for( size_t index = 0; index != m_cursors.size(); ++index )
        {
            int shape = Detail::getStandardCursorShape_( (Mosaic::CursorShape)index );

            m_cursors[index] = glfwCreateStandardCursor( shape );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::finalize()
    {
        for( GLFWcursor * cursor : m_cursors )
        {
            if( cursor == nullptr )
            {
                continue;
            }

            glfwDestroyCursor( cursor );
        }

        m_cursors.fill( nullptr );

        m_window = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::getClipboardText( Mosaic::String * const _out )
    {
        const char * text = glfwGetClipboardString( m_window );

        if( text == nullptr )
        {
            return false;
        }

        _out->assign( text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::setClipboardText( Mosaic::StringView _text )
    {
        Mosaic::String text = Detail::makePath_( _text );

        glfwSetClipboardString( m_window, text.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::writeConsole( Mosaic::StringView _text )
    {
        std::fprintf( stdout, "%.*s\n", (int)_text.size(), _text.data() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::readFile( Mosaic::StringView _path, Mosaic::ByteVector * const _out )
    {
        Mosaic::String path = Detail::makePath_( _path );

        std::FILE * file = std::fopen( path.c_str(), "rb" );

        if( file == nullptr )
        {
            return false;
        }

        std::fseek( file, 0, SEEK_END );
        long size = std::ftell( file );
        std::fseek( file, 0, SEEK_SET );

        if( size < 0 )
        {
            std::fclose( file );

            return false;
        }

        _out->resize( (size_t)size );

        size_t read = size == 0 ? 0 : std::fread( _out->data(), 1, (size_t)size, file );

        std::fclose( file );

        if( read != (size_t)size )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::writeFile( Mosaic::StringView _path, Mosaic::ByteSpan _data )
    {
        Mosaic::String path = Detail::makePath_( _path );

        std::FILE * file = std::fopen( path.c_str(), "wb" );

        if( file == nullptr )
        {
            return false;
        }

        size_t written = _data.empty() == true ? 0 : std::fwrite( _data.data(), 1, _data.size(), file );

        std::fclose( file );

        if( written != _data.size() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicPlatform::userDataPath( Mosaic::StringView _application, Mosaic::StringView _filename, Mosaic::String * const _out )
    {
        const char * home = std::getenv( "HOME" );

        if( home == nullptr )
        {
            home = std::getenv( "USERPROFILE" );
        }

        if( home == nullptr )
        {
            return false;
        }

        Mosaic::String path( home );
        path += '/';
        path.append( _application.data(), _application.size() );
        path += '_';
        path.append( _filename.data(), _filename.size() );

        *_out = path;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    double ToolMosaicPlatform::monotonicTime() const noexcept
    {
        double time = glfwGetTime();

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::setCursor( Mosaic::CursorShape _cursor )
    {
        if( m_cursor == _cursor )
        {
            return;
        }

        m_cursor = _cursor;

        size_t index = (size_t)_cursor;

        if( index >= m_cursors.size() )
        {
            return;
        }

        GLFWcursor * cursor = m_cursors[index];

        if( cursor == nullptr )
        {
            return;
        }

        glfwSetCursor( m_window, cursor );
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::setImeCandidateRect( const Mosaic::Rect & _screenRect )
    {
        MENGINE_UNUSED( _screenRect );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void * ToolMosaicPlatform::createWindow( const Mosaic::NativeWindowDescription & _description )
    {
        MENGINE_UNUSED( _description );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::destroyWindow( void * _nativeHandle )
    {
        MENGINE_UNUSED( _nativeHandle );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::showWindow( void * _nativeHandle, bool _visible )
    {
        MENGINE_UNUSED( _nativeHandle );
        MENGINE_UNUSED( _visible );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::setWindowBounds( void * _nativeHandle, const Mosaic::Rect & _bounds )
    {
        MENGINE_UNUSED( _nativeHandle );
        MENGINE_UNUSED( _bounds );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::MonitorSpan ToolMosaicPlatform::monitors() const noexcept
    {
        GLFWmonitor * monitor = glfwGetPrimaryMonitor();

        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;

        if( monitor != nullptr )
        {
            glfwGetMonitorWorkarea( monitor, &x, &y, &width, &height );
        }

        m_monitor.id = 1;
        m_monitor.bounds = {(float)x, (float)y, (float)width, (float)height};
        m_monitor.workArea = m_monitor.bounds;
        m_monitor.dpiScale = 1.f;
        m_monitor.primary = true;

        Mosaic::MonitorSpan monitors( &m_monitor, 1 );

        return monitors;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicPlatform::publishAccessibilityTree( Mosaic::SemanticNodeSpan _semantics )
    {
        MENGINE_UNUSED( _semantics );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
