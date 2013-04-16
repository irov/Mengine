#   pragma once

#   include "Interface/WindowsLayerInterface.h"

#   include "Config/Typedef.h"

#	include "WindowsIncluder.h"

namespace Menge
{
    class VistaWindowsLayer
        : public WindowsLayerInterface
    {
    public:
        VistaWindowsLayer();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        DynamicLibraryInterface * loadDynamicLibrary( const WString & _path ) override;

    public:
        EWindowsType getWindowsType() override;

        bool setProcessDPIAware() override;
        bool supportUnicode() override;

        bool setCurrentDirectory( const WString & _path ) override;
        bool setModuleCurrentDirectory() override;
        bool createDirectory( const WString & _path ) override;
        bool fileExists( const WString & _path ) override;

        HANDLE createFile( const WString & _filename, DWORD _desiredAccess,
            DWORD _sharedMode, DWORD _creationDisposition ) override;

        ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const WString& _className ) override;

        bool unregisterClass( const WString& _className, HINSTANCE _hInstance ) override;

        HWND createWindow( const WString& _className, const WString& _windowName
            , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance, LPVOID _param ) override;

        HWND createWindowEx( DWORD _exStyle, const WString& _className
            , const WString& _windowName,	DWORD _style, int _x, int _y
            , int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance,	LPVOID _param ) override;

        void destroyWindow( HWND _hwnd ) override;
        LPVOID getCreateParams( LPARAM _lParam ) override;
        LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong ) override;
        LONG getWindowLong( HWND _hWnd,	int _index ) override;
        LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong ) override;
        LONG_PTR getWindowLongPtr( HWND _hWnd, int _index ) override;
        LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam ) override;
        bool getCurrentDirectory( WString & _path ) override;
        bool peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax, UINT _removeMsg ) override;
        LRESULT dispatchMessage( const MSG* _msg ) override;

        int messageBox( HWND _hWnd, const WString& _text, const WString& _caption, UINT _type ) override;

        bool getModuleFileName( HMODULE hModule, WString & _moduleFilename ) override;
        //LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
        //LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName );

        bool makeFormatMessage( HRESULT _hresult, WString & _out ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        EWindowsType m_windowsType;

        bool m_checkedUnicode;
        bool m_supportUnicode;
    };
}