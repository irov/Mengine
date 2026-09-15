#pragma once

#include "Mosaic/Platform.hpp"

#include <array>

struct GLFWwindow;
struct GLFWcursor;

namespace Mengine
{
    class ToolMosaicPlatform
        : public Mosaic::PlatformAdapter
    {
    public:
        ToolMosaicPlatform();
        ~ToolMosaicPlatform() override;

    public:
        bool initialize( GLFWwindow * _window );
        void finalize();

    public:
        bool getClipboardText( Mosaic::String * const _out ) override;
        void setClipboardText( Mosaic::StringView _text ) override;
        bool writeConsole( Mosaic::StringView _text ) override;
        bool readFile( Mosaic::StringView _path, Mosaic::ByteVector * const _out ) override;
        bool writeFile( Mosaic::StringView _path, Mosaic::ByteSpan _data ) override;
        bool userDataPath( Mosaic::StringView _application, Mosaic::StringView _filename, Mosaic::String * const _out ) override;
        double monotonicTime() const noexcept override;
        void setCursor( Mosaic::CursorShape _cursor ) override;
        void setImeCandidateRect( const Mosaic::Rect & _screenRect ) override;

    public:
        void * createWindow( const Mosaic::NativeWindowDescription & _description ) override;
        void destroyWindow( void * _nativeHandle ) override;
        void showWindow( void * _nativeHandle, bool _visible ) override;
        void setWindowBounds( void * _nativeHandle, const Mosaic::Rect & _bounds ) override;

    public:
        Mosaic::MonitorSpan monitors() const noexcept override;
        void publishAccessibilityTree( Mosaic::SemanticNodeSpan _semantics ) override;

    protected:
        GLFWwindow * m_window;

        typedef std::array<GLFWcursor *, 12> ArrayCursors;
        ArrayCursors m_cursors;

        Mosaic::CursorShape m_cursor;

        mutable Mosaic::Monitor m_monitor;
    };
}
