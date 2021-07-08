#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#ifndef MENGINE_UWM_MOUSE_LEAVE
#define MENGINE_UWM_MOUSE_LEAVE (WM_USER+1)
#endif

#ifndef MENGINE_UTIMER_MOUSE_EVENT
#define MENGINE_UTIMER_MOUSE_EVENT 1001
#endif

namespace Mengine
{
    class Win32MouseEvent
    {
    public:
        Win32MouseEvent();
        ~Win32MouseEvent();

    public:
        void setHWND( HWND _hWnd );

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