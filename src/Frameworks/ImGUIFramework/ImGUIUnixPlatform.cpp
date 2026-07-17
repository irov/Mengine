#include "ImGUIUnixPlatform.h"

#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include "Interface/ApplicationInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/TimestampHelper.h"

#include "imgui.h"

#include <cfloat>
#include <cstdint>
#include <cstring>

namespace
{
    //////////////////////////////////////////////////////////////////////////
    struct MengineImGUIUnixPlatformData
    {
        MengineImGUIUnixPlatformData()
            : display( nullptr )
            , window( 0 )
            , time( 0.0 )
            , invisibleCursor( 0 )
            , lastCursor( ImGuiMouseCursor_COUNT )
        {
            std::memset( mouseCursors, 0, sizeof( mouseCursors ) );
        }

        Display * display;
        Window window;
        double time;
        Cursor mouseCursors[ImGuiMouseCursor_COUNT];
        Cursor invisibleCursor;
        ImGuiMouseCursor lastCursor;
        ImVector<char> clipboard;
    };
    //////////////////////////////////////////////////////////////////////////
    static MengineImGUIUnixPlatformData * getBackendData()
    {
        if( ImGui::GetCurrentContext() == nullptr )
        {
            return nullptr;
        }

        ImGuiIO & io = ImGui::GetIO();

        return static_cast<MengineImGUIUnixPlatformData *>(io.BackendPlatformUserData);
    }
    //////////////////////////////////////////////////////////////////////////
    static ImGuiKey keySymToImGuiKey( KeySym _keySym )
    {
        switch( _keySym )
        {
        case XK_Tab:
        case XK_ISO_Left_Tab: return ImGuiKey_Tab;
        case XK_Left: return ImGuiKey_LeftArrow;
        case XK_Right: return ImGuiKey_RightArrow;
        case XK_Up: return ImGuiKey_UpArrow;
        case XK_Down: return ImGuiKey_DownArrow;
        case XK_Prior: return ImGuiKey_PageUp;
        case XK_Next: return ImGuiKey_PageDown;
        case XK_Home: return ImGuiKey_Home;
        case XK_End: return ImGuiKey_End;
        case XK_Insert: return ImGuiKey_Insert;
        case XK_Delete: return ImGuiKey_Delete;
        case XK_BackSpace: return ImGuiKey_Backspace;
        case XK_space: return ImGuiKey_Space;
        case XK_Return: return ImGuiKey_Enter;
        case XK_Escape: return ImGuiKey_Escape;
        case XK_apostrophe: return ImGuiKey_Apostrophe;
        case XK_comma: return ImGuiKey_Comma;
        case XK_minus: return ImGuiKey_Minus;
        case XK_period: return ImGuiKey_Period;
        case XK_slash: return ImGuiKey_Slash;
        case XK_semicolon: return ImGuiKey_Semicolon;
        case XK_equal: return ImGuiKey_Equal;
        case XK_bracketleft: return ImGuiKey_LeftBracket;
        case XK_backslash: return ImGuiKey_Backslash;
        case XK_bracketright: return ImGuiKey_RightBracket;
        case XK_grave: return ImGuiKey_GraveAccent;
        case XK_Caps_Lock: return ImGuiKey_CapsLock;
        case XK_Scroll_Lock: return ImGuiKey_ScrollLock;
        case XK_Num_Lock: return ImGuiKey_NumLock;
        case XK_Print: return ImGuiKey_PrintScreen;
        case XK_Pause: return ImGuiKey_Pause;
        case XK_KP_0: return ImGuiKey_Keypad0;
        case XK_KP_1: return ImGuiKey_Keypad1;
        case XK_KP_2: return ImGuiKey_Keypad2;
        case XK_KP_3: return ImGuiKey_Keypad3;
        case XK_KP_4: return ImGuiKey_Keypad4;
        case XK_KP_5: return ImGuiKey_Keypad5;
        case XK_KP_6: return ImGuiKey_Keypad6;
        case XK_KP_7: return ImGuiKey_Keypad7;
        case XK_KP_8: return ImGuiKey_Keypad8;
        case XK_KP_9: return ImGuiKey_Keypad9;
        case XK_KP_Decimal: return ImGuiKey_KeypadDecimal;
        case XK_KP_Divide: return ImGuiKey_KeypadDivide;
        case XK_KP_Multiply: return ImGuiKey_KeypadMultiply;
        case XK_KP_Subtract: return ImGuiKey_KeypadSubtract;
        case XK_KP_Add: return ImGuiKey_KeypadAdd;
        case XK_KP_Enter: return ImGuiKey_KeypadEnter;
        case XK_KP_Equal: return ImGuiKey_KeypadEqual;
        case XK_Shift_L: return ImGuiKey_LeftShift;
        case XK_Control_L: return ImGuiKey_LeftCtrl;
        case XK_Alt_L:
        case XK_Meta_L: return ImGuiKey_LeftAlt;
        case XK_Super_L: return ImGuiKey_LeftSuper;
        case XK_Shift_R: return ImGuiKey_RightShift;
        case XK_Control_R: return ImGuiKey_RightCtrl;
        case XK_Alt_R:
        case XK_Meta_R: return ImGuiKey_RightAlt;
        case XK_Super_R: return ImGuiKey_RightSuper;
        case XK_Menu: return ImGuiKey_Menu;
        case XK_0: return ImGuiKey_0;
        case XK_1: return ImGuiKey_1;
        case XK_2: return ImGuiKey_2;
        case XK_3: return ImGuiKey_3;
        case XK_4: return ImGuiKey_4;
        case XK_5: return ImGuiKey_5;
        case XK_6: return ImGuiKey_6;
        case XK_7: return ImGuiKey_7;
        case XK_8: return ImGuiKey_8;
        case XK_9: return ImGuiKey_9;
        case XK_A:
        case XK_a: return ImGuiKey_A;
        case XK_B:
        case XK_b: return ImGuiKey_B;
        case XK_C:
        case XK_c: return ImGuiKey_C;
        case XK_D:
        case XK_d: return ImGuiKey_D;
        case XK_E:
        case XK_e: return ImGuiKey_E;
        case XK_F:
        case XK_f: return ImGuiKey_F;
        case XK_G:
        case XK_g: return ImGuiKey_G;
        case XK_H:
        case XK_h: return ImGuiKey_H;
        case XK_I:
        case XK_i: return ImGuiKey_I;
        case XK_J:
        case XK_j: return ImGuiKey_J;
        case XK_K:
        case XK_k: return ImGuiKey_K;
        case XK_L:
        case XK_l: return ImGuiKey_L;
        case XK_M:
        case XK_m: return ImGuiKey_M;
        case XK_N:
        case XK_n: return ImGuiKey_N;
        case XK_O:
        case XK_o: return ImGuiKey_O;
        case XK_P:
        case XK_p: return ImGuiKey_P;
        case XK_Q:
        case XK_q: return ImGuiKey_Q;
        case XK_R:
        case XK_r: return ImGuiKey_R;
        case XK_S:
        case XK_s: return ImGuiKey_S;
        case XK_T:
        case XK_t: return ImGuiKey_T;
        case XK_U:
        case XK_u: return ImGuiKey_U;
        case XK_V:
        case XK_v: return ImGuiKey_V;
        case XK_W:
        case XK_w: return ImGuiKey_W;
        case XK_X:
        case XK_x: return ImGuiKey_X;
        case XK_Y:
        case XK_y: return ImGuiKey_Y;
        case XK_Z:
        case XK_z: return ImGuiKey_Z;
        case XK_F1: return ImGuiKey_F1;
        case XK_F2: return ImGuiKey_F2;
        case XK_F3: return ImGuiKey_F3;
        case XK_F4: return ImGuiKey_F4;
        case XK_F5: return ImGuiKey_F5;
        case XK_F6: return ImGuiKey_F6;
        case XK_F7: return ImGuiKey_F7;
        case XK_F8: return ImGuiKey_F8;
        case XK_F9: return ImGuiKey_F9;
        case XK_F10: return ImGuiKey_F10;
        case XK_F11: return ImGuiKey_F11;
        case XK_F12: return ImGuiKey_F12;
        case XK_F13: return ImGuiKey_F13;
        case XK_F14: return ImGuiKey_F14;
        case XK_F15: return ImGuiKey_F15;
        case XK_F16: return ImGuiKey_F16;
        case XK_F17: return ImGuiKey_F17;
        case XK_F18: return ImGuiKey_F18;
        case XK_F19: return ImGuiKey_F19;
        case XK_F20: return ImGuiKey_F20;
        case XK_F21: return ImGuiKey_F21;
        case XK_F22: return ImGuiKey_F22;
        case XK_F23: return ImGuiKey_F23;
        case XK_F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static void updateModifiers( ImGuiIO & _io, unsigned int _state, KeySym _keySym, bool _down )
    {
        bool shift = (_state & ShiftMask) != 0;
        bool control = (_state & ControlMask) != 0;
        bool alt = (_state & Mod1Mask) != 0;
        bool super = (_state & Mod4Mask) != 0;

        switch( _keySym )
        {
        case XK_Shift_L:
        case XK_Shift_R: shift = _down; break;
        case XK_Control_L:
        case XK_Control_R: control = _down; break;
        case XK_Alt_L:
        case XK_Alt_R:
        case XK_Meta_L:
        case XK_Meta_R: alt = _down; break;
        case XK_Super_L:
        case XK_Super_R: super = _down; break;
        default: break;
        }

        _io.AddKeyEvent( ImGuiMod_Shift, shift );
        _io.AddKeyEvent( ImGuiMod_Ctrl, control );
        _io.AddKeyEvent( ImGuiMod_Alt, alt );
        _io.AddKeyEvent( ImGuiMod_Super, super );
    }
    //////////////////////////////////////////////////////////////////////////
    static int mouseButtonToImGuiButton( unsigned int _button )
    {
        switch( _button )
        {
        case Button1: return ImGuiMouseButton_Left;
        case Button2: return ImGuiMouseButton_Middle;
        case Button3: return ImGuiMouseButton_Right;
        case 8: return 3;
        case 9: return 4;
        default: return -1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static void updateMouseCursor( MengineImGUIUnixPlatformData * _data )
    {
        ImGuiIO & io = ImGui::GetIO();

        if( SERVICE_IS_INITIALIZE( Mengine::ApplicationInterface ) == true )
        {
            if( APPLICATION_SERVICE()->getCursorMode() == false )
            {
                if( _data->lastCursor != ImGuiMouseCursor_None )
                {
                    PLATFORM_SERVICE()
                        ->notifyCursorModeChanged( false );

                    _data->lastCursor = ImGuiMouseCursor_None;
                }

                return;
            }
        }

        if( (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) != 0 )
        {
            return;
        }

        ImGuiMouseCursor cursor = io.MouseDrawCursor == true
            ? ImGuiMouseCursor_None
            : ImGui::GetMouseCursor();

        if( cursor == _data->lastCursor )
        {
            return;
        }

        _data->lastCursor = cursor;

        if( cursor == ImGuiMouseCursor_None )
        {
            if( _data->invisibleCursor != 0 )
            {
                XDefineCursor( _data->display, _data->window, _data->invisibleCursor );
            }
        }
        else
        {
            Cursor xcursor = _data->mouseCursors[cursor];

            if( xcursor == 0 )
            {
                xcursor = _data->mouseCursors[ImGuiMouseCursor_Arrow];
            }

            XDefineCursor( _data->display, _data->window, xcursor );
        }

        XFlush( _data->display );
    }
    //////////////////////////////////////////////////////////////////////////
    static const char * getClipboardText()
    {
        MengineImGUIUnixPlatformData * data = getBackendData();

        if( data == nullptr )
        {
            return nullptr;
        }

        constexpr int clipboardCapacity = 64 * 1024;
        data->clipboard.resize( clipboardCapacity );

        if( PLATFORM_SERVICE()->getClipboardText( data->clipboard.Data, data->clipboard.Size ) == false )
        {
            data->clipboard.clear();

            return nullptr;
        }

        size_t clipboardSize = std::strlen( data->clipboard.Data );
        data->clipboard.resize( static_cast<int>(clipboardSize + 1) );

        return data->clipboard.Data;
    }
}
//////////////////////////////////////////////////////////////////////////
bool MengineImGUIUnixPlatform_Init( Display * _display, Window _window )
{
    IMGUI_CHECKVERSION();

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    if( io.BackendPlatformUserData != nullptr )
    {
        return false;
    }

    if( _display == nullptr || _window == 0 )
    {
        return false;
    }

    MengineImGUIUnixPlatformData * data = IM_NEW( MengineImGUIUnixPlatformData )();
    data->display = _display;
    data->window = _window;

    io.BackendPlatformUserData = data;
    io.BackendPlatformName = "mengine_imgui_unix";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGuiViewport * mainViewport = ImGui::GetMainViewport();
    mainViewport->PlatformHandle = reinterpret_cast<void *>(static_cast<uintptr_t>(_window));
    mainViewport->PlatformHandleRaw = mainViewport->PlatformHandle;

    data->mouseCursors[ImGuiMouseCursor_Arrow] = XCreateFontCursor( _display, XC_left_ptr );
    data->mouseCursors[ImGuiMouseCursor_TextInput] = XCreateFontCursor( _display, XC_xterm );
    data->mouseCursors[ImGuiMouseCursor_ResizeAll] = XCreateFontCursor( _display, XC_fleur );
    data->mouseCursors[ImGuiMouseCursor_ResizeNS] = XCreateFontCursor( _display, XC_sb_v_double_arrow );
    data->mouseCursors[ImGuiMouseCursor_ResizeEW] = XCreateFontCursor( _display, XC_sb_h_double_arrow );
    data->mouseCursors[ImGuiMouseCursor_ResizeNESW] = XCreateFontCursor( _display, XC_bottom_left_corner );
    data->mouseCursors[ImGuiMouseCursor_ResizeNWSE] = XCreateFontCursor( _display, XC_bottom_right_corner );
    data->mouseCursors[ImGuiMouseCursor_Hand] = XCreateFontCursor( _display, XC_hand2 );
    data->mouseCursors[ImGuiMouseCursor_Wait] = XCreateFontCursor( _display, XC_watch );
    data->mouseCursors[ImGuiMouseCursor_Progress] = XCreateFontCursor( _display, XC_watch );
    data->mouseCursors[ImGuiMouseCursor_NotAllowed] = XCreateFontCursor( _display, XC_X_cursor );

    const char emptyCursorData[1] = {0};
    Pixmap emptyCursorPixmap = XCreateBitmapFromData( _display, _window, emptyCursorData, 1, 1 );

    if( emptyCursorPixmap != 0 )
    {
        XColor emptyCursorColor = {};
        data->invisibleCursor = XCreatePixmapCursor( _display, emptyCursorPixmap, emptyCursorPixmap, &emptyCursorColor, &emptyCursorColor, 0, 0 );
        XFreePixmap( _display, emptyCursorPixmap );
    }

    platformIO.Platform_SetClipboardTextFn = []( ImGuiContext *, const char * _text )
    {
        PLATFORM_SERVICE()
            ->setClipboardText( _text );
    };

    platformIO.Platform_GetClipboardTextFn = []( ImGuiContext * ) -> const char *
    {
        return getClipboardText();
    };

    return true;
}
//////////////////////////////////////////////////////////////////////////
void MengineImGUIUnixPlatform_Shutdown()
{
    MengineImGUIUnixPlatformData * data = getBackendData();

    if( data == nullptr )
    {
        return;
    }

    XUndefineCursor( data->display, data->window );

    for( Cursor & cursor : data->mouseCursors )
    {
        if( cursor != 0 )
        {
            XFreeCursor( data->display, cursor );
            cursor = 0;
        }
    }

    if( data->invisibleCursor != 0 )
    {
        XFreeCursor( data->display, data->invisibleCursor );
        data->invisibleCursor = 0;
    }

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();
    ImGuiViewport * mainViewport = ImGui::GetMainViewport();

    mainViewport->PlatformHandle = nullptr;
    mainViewport->PlatformHandleRaw = nullptr;

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos);

    platformIO.ClearPlatformHandlers();

    IM_DELETE( data );
}
//////////////////////////////////////////////////////////////////////////
bool MengineImGUIUnixPlatform_IsInitialized()
{
    if( getBackendData() != nullptr )
    {
        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void MengineImGUIUnixPlatform_NewFrame()
{
    MengineImGUIUnixPlatformData * data = getBackendData();
    IM_ASSERT( data != nullptr && "ImGUI Unix platform backend is not initialized" );

    ImGuiIO & io = ImGui::GetIO();

    XWindowAttributes windowAttributes;

    if( XGetWindowAttributes( data->display, data->window, &windowAttributes ) != 0 )
    {
        io.DisplaySize = ImVec2( static_cast<float>(windowAttributes.width), static_cast<float>(windowAttributes.height) );
        io.DisplayFramebufferScale = ImVec2( 1.f, 1.f );
    }

    if( io.WantSetMousePos == true )
    {
        XWarpPointer( data->display, 0, data->window, 0, 0, 0, 0, static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y) );
    }

    double currentTime = Mengine::Helper::getElapsedTime();
    io.DeltaTime = data->time > 0.0
        ? static_cast<float>((currentTime - data->time) * 0.001)
        : 1.f / 60.f;
    data->time = currentTime;

    updateMouseCursor( data );
}
//////////////////////////////////////////////////////////////////////////
bool MengineImGUIUnixPlatform_ProcessEvent( const XEvent & _event, const char * _text )
{
    MengineImGUIUnixPlatformData * data = getBackendData();

    if( data == nullptr || _event.xany.window != data->window )
    {
        return false;
    }

    ImGuiIO & io = ImGui::GetIO();

    switch( _event.type )
    {
    case MotionNotify:
        io.AddMouseSourceEvent( ImGuiMouseSource_Mouse );
        io.AddMousePosEvent( static_cast<float>(_event.xmotion.x), static_cast<float>(_event.xmotion.y) );
        return io.WantCaptureMouse;
    case EnterNotify:
        io.AddMouseSourceEvent( ImGuiMouseSource_Mouse );
        io.AddMousePosEvent( static_cast<float>(_event.xcrossing.x), static_cast<float>(_event.xcrossing.y) );
        return io.WantCaptureMouse;
    case LeaveNotify:
        io.AddMousePosEvent( -FLT_MAX, -FLT_MAX );
        return io.WantCaptureMouse;
    case ButtonPress:
    case ButtonRelease:
        {
            bool down = _event.type == ButtonPress;
            unsigned int button = _event.xbutton.button;

            io.AddMouseSourceEvent( ImGuiMouseSource_Mouse );
            io.AddMousePosEvent( static_cast<float>(_event.xbutton.x), static_cast<float>(_event.xbutton.y) );

            if( down == true )
            {
                switch( button )
                {
                case Button4: io.AddMouseWheelEvent( 0.f, 1.f ); return io.WantCaptureMouse;
                case Button5: io.AddMouseWheelEvent( 0.f, -1.f ); return io.WantCaptureMouse;
                case 6: io.AddMouseWheelEvent( -1.f, 0.f ); return io.WantCaptureMouse;
                case 7: io.AddMouseWheelEvent( 1.f, 0.f ); return io.WantCaptureMouse;
                default: break;
                }
            }

            int imguiButton = mouseButtonToImGuiButton( button );

            if( imguiButton != -1 )
            {
                io.AddMouseButtonEvent( imguiButton, down );
            }

            return io.WantCaptureMouse;
        }
    case KeyPress:
    case KeyRelease:
        {
            bool down = _event.type == KeyPress;
            bool captureKeyboard = io.WantCaptureKeyboard;
            XKeyEvent keyEvent = _event.xkey;
            KeySym keySym = XLookupKeysym( &keyEvent, 0 );
            ImGuiKey key = keySymToImGuiKey( keySym );

            updateModifiers( io, keyEvent.state, keySym, down );

            if( key != ImGuiKey_None )
            {
                io.AddKeyEvent( key, down );
                io.SetKeyEventNativeData( key, static_cast<int>(keyEvent.keycode), static_cast<int>(keySym) );
            }

            if( down == true && _text != nullptr && (keyEvent.state & (ControlMask | Mod1Mask | Mod4Mask)) == 0 )
            {
                io.AddInputCharactersUTF8( _text );
            }

            return captureKeyboard;
        }
    case FocusIn:
        io.AddFocusEvent( true );
        return false;
    case FocusOut:
        io.AddFocusEvent( false );
        return false;
    default:
        return false;
    }
}
