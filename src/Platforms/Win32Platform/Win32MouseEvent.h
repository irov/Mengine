#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#define UWM_MOUSE_LEAVE (WM_USER+1)

#define MENGINE_UTIMER_MOUSE_EVENT 1001

namespace Mengine
{
    class Win32MouseEvent
    {
    public:
        Win32MouseEvent();
        ~Win32MouseEvent();

    public:
        bool initialize( HWND _hWnd );

    public:
        void verify();
        void update();

    public:
        void stop();

    protected:
        HWND m_hWnd;

        UINT_PTR m_uTimer;
    };
}