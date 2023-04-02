#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Document.h"

#include "Config/Typedef.h"
#include "Config/Lambda.h"
#include "Config/Thread.h"

namespace Mengine
{
    class Win32PlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual bool atachWindow( HWND _hwnd, bool _fullscreen ) = 0;

    public:
        virtual HWND getWindowHandle() const = 0;

    public:
        typedef Lambda<LRESULT( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL * const pHandled )> LambdaWin32ProcessHandler;
        virtual UniqueId addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentPtr & _doc ) = 0;
        virtual void removeWin32ProcessHandler( UniqueId _id ) = 0;

    public:
        virtual bool setHWNDIcon( const WChar * _iconResource ) = 0;

    public:
        virtual size_t getShortPathName( const Char * _filePath, Char * const _shortFilePath ) const = 0;

    public:
        virtual time_t getFileUnixTime( const FILETIME * filetime ) const = 0;

    public:
        virtual bool getLocalMachineRegValue( const Char * _path, const Char * _key, Char * const _value, size_t _size ) = 0;

    public:
        virtual bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * const _exitCode ) = 0;

    public:
        virtual bool getCallstack( ThreadId _threadId, Char * const _stack, size_t _capacity, PCONTEXT _context ) const = 0;
    };
}