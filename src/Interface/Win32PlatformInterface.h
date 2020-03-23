#pragma once

#include "Config/Typedef.h"
#include "Config/Lambda.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32PlatformInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<LRESULT( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )> LambdaWin32ProcessHandler;
        virtual uint32_t addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda ) = 0;
        virtual void removeWin32ProcessHandler( uint32_t _id ) = 0;
    };
}