#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/DocumentHelper.h"

#include "Config/Typedef.h"
#include "Config/Lambda.h"
#include "Config/ThreadId.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    class UWPPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual bool atachWindow( HWND _hwnd, bool _fullscreen ) = 0;

    public:
        virtual HWND getWindowHandle() const = 0;

    public:
        typedef Lambda<LRESULT( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL * const pHandled )> LambdaWin32ProcessHandler;
        virtual UniqueId addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeWin32ProcessHandler( UniqueId _id ) = 0;

    public:
        virtual bool setHWNDIcon( const WChar * _iconResource ) = 0;

    public:
        virtual size_t getShortPathName( const Char * _filePath, Char * const _shortFilePath ) const = 0;
    };
}