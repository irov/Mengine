#   pragma once

#   include "Config/Typedef.h"
#   include "Config/String.h"

#   include "Interface/ServiceInterface.h"

#   include "Core/FilePath.h"

#   include <wtypes.h>

namespace Menge
{
    enum EWindowsType
    {
        EWT_UNKNOWN,
        EWT_NT,
        EWT_98,
        EWT_VISTA
    };

    typedef void * (*TDynamicLibraryFunction)( void * );

    class DynamicLibraryInterface
    {
    public:
        virtual bool load() = 0;
        virtual void destroy() = 0;

    public:
        virtual TDynamicLibraryFunction getSymbol( const String & _name ) const = 0;
    };

    class WindowsLayerInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE("WindowsLayerService")

    public:
        virtual DynamicLibraryInterface * loadDynamicLibrary( const WString & _name ) = 0;

    public:
        virtual EWindowsType getWindowsType() = 0;
        virtual bool setProcessDPIAware() = 0;
        virtual bool supportUnicode() = 0;

        virtual bool setCurrentDirectory( const WChar * _path ) = 0;
        virtual bool setModuleCurrentDirectory() = 0;
        virtual bool createDirectory( const WChar * _path ) = 0;
        virtual bool fileExists( const WChar * _path ) = 0;

        virtual HANDLE createFile( const WChar * _fileName, DWORD _desiredAccess,
            DWORD _sharedMode, DWORD _creationDisposition ) = 0;

        virtual ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const WChar * _className ) = 0;

        virtual bool unregisterClass( const WChar * _className, HINSTANCE _hInstance ) = 0;

        virtual HWND createWindow( const WChar * _className, const WChar * _windowName
            , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance, LPVOID _param ) = 0;

        virtual HWND createWindowEx( DWORD _exStyle, const WChar * _className
            , const WChar * _windowName,	DWORD _style, int _x, int _y
            , int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance,	LPVOID _param ) = 0;

        virtual void destroyWindow( HWND _hwnd ) = 0;
        virtual LPVOID getCreateParams( LPARAM _lParam ) = 0;
        virtual LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong ) = 0;
        virtual LONG getWindowLong( HWND _hWnd,	int _index ) = 0;
        virtual LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong ) = 0;
        virtual LONG_PTR getWindowLongPtr( HWND _hWnd, int _index ) = 0;
        virtual LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam ) = 0;
		virtual size_t getCurrentDirectory( WChar * _path, size_t _len ) = 0;
        virtual bool peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax, UINT _removeMsg ) = 0;
        virtual LRESULT dispatchMessage( const MSG* _msg ) = 0;

        virtual int messageBox( HWND _hWnd, const WChar * _text, const WChar * _caption, UINT _type ) = 0;

        virtual bool getModuleFileName( HMODULE hModule, WChar * _moduleFilename, size_t _capacity ) = 0;
        //LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
        //LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName );

        virtual bool makeFormatMessage( HRESULT _hresult, WString & _out ) = 0;
        virtual bool concatenateFilePath( const ConstString & _folder, const ConstString & _fileName, WChar * _filePath, size_t _capacity ) = 0;

		virtual bool cmd( const WString & _command ) = 0;

		virtual bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) = 0;
		virtual bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) = 0;
    };

#   define WINDOWSLAYER_SERVICE( serviceProvider )\
    ((Menge::WindowsLayerInterface*)SERVICE_GET(serviceProvider, Menge::WindowsLayerInterface))
}