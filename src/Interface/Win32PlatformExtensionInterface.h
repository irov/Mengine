#pragma once

#include "Interface/UnknownInterface.h"

#include "Config/Typedef.h"
#include "Config/Lambda.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32PlatformExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual HWND getWindowHandle() const = 0;

    public:
        typedef Lambda<LRESULT( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )> LambdaWin32ProcessHandler;
        virtual uint32_t addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda ) = 0;
        virtual void removeWin32ProcessHandler( uint32_t _id ) = 0;

    public:
        virtual size_t getShortPathName( const Char * _filePath, Char * const _shortFilePath ) const = 0;

    public:
        virtual time_t getFileUnixTime( const FILETIME * filetime ) const = 0;

    public:
        virtual bool getErrorMessage( DWORD _messageId, Char * const _out, size_t _capacity ) const = 0;

    public:
        virtual bool getLocalMachineRegValue( const Char * _path, const Char * _key, Char * const _value, size_t _size ) = 0;

    public:
        virtual bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * const _exitCode ) = 0;
    };
}