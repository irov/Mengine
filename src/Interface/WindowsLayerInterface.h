#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "wtypes.h"

#   include "Config/Typedef.h"

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
        virtual TDynamicLibraryFunction getSymbol( const String& _name ) const = 0;
    };

    class WindowsLayerInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE("WindowsLayerService")

    public:
        virtual DynamicLibraryInterface * loadDynamicLibrary( const WString & _path ) = 0;

    public:
        virtual EWindowsType getWindowsType() = 0;
        virtual bool setProcessDPIAware() = 0;
        virtual bool supportUnicode() = 0;

        virtual bool setCurrentDirectory( const WString & _path ) = 0;
        virtual bool setModuleCurrentDirectory() = 0;
        virtual bool createDirectory( const WString & _path ) = 0;
        virtual bool fileExists( const WString & _path ) = 0;

        virtual HANDLE createFile( const WString & _filename, DWORD _desiredAccess,
            DWORD _sharedMode, DWORD _creationDisposition ) = 0;

        virtual ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const Menge::WString& _className ) = 0;

        virtual bool unregisterClass( const WString& _className, HINSTANCE _hInstance ) = 0;

        virtual HWND createWindow( const WString& _className, const WString& _windowName
            , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance, LPVOID _param ) = 0;

        virtual HWND createWindowEx( DWORD _exStyle, const WString& _className
            , const WString& _windowName,	DWORD _style, int _x, int _y
            , int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance,	LPVOID _param ) = 0;

        virtual void destroyWindow( HWND _hwnd ) = 0;
        virtual LPVOID getCreateParams( LPARAM _lParam ) = 0;
        virtual LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong ) = 0;
        virtual LONG getWindowLong( HWND _hWnd,	int _index ) = 0;
        virtual LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong ) = 0;
        virtual LONG_PTR getWindowLongPtr( HWND _hWnd, int _index ) = 0;
        virtual LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam ) = 0;
        virtual bool getCurrentDirectory( WString & _path ) = 0;
        virtual bool peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax, UINT _removeMsg ) = 0;
        virtual LRESULT dispatchMessage( const MSG* _msg ) = 0;

        virtual int messageBox( HWND _hWnd, const WString & _text, const WString & _caption, UINT _type ) = 0;

        virtual bool getModuleFileName( HMODULE hModule, WString & _moduleFilename ) = 0;
        //LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
        //LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName );

        virtual bool makeFormatMessage( HRESULT _hresult, WString & _out ) = 0;
    };

#   define WINDOWSLAYER_SERVICE( serviceProvider )\
    (Menge::getService<Menge::WindowsLayerInterface>(serviceProvider))
}