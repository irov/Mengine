#   pragma once

#   include "Interface/WindowsLayerInterface.h"

#   include "Config/Typedef.h"

#   include "Core/ServiceBase.h"

#	include "WIN32/WindowsIncluder.h"

namespace Menge
{
    class VistaWindowsLayer
        : public ServiceBase<WindowsLayerInterface>
    {
    public:
        VistaWindowsLayer();

    public:
        EWindowsType getWindowsType() override;

        bool setProcessDPIAware() override;
        bool supportUnicode() override;

        bool fileExists( const WChar * _path ) override;

        HANDLE createFile( const WChar * _fileName, DWORD _desiredAccess,
            DWORD _sharedMode, DWORD _creationDisposition ) override;
						
        ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const WChar * _className ) override;

        bool unregisterClass( const WChar * _className, HINSTANCE _hInstance ) override;

        HWND createWindow( const WChar * _className, const WChar * _windowName
            , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance, LPVOID _param ) override;

        HWND createWindowEx( DWORD _exStyle, const WChar * _className
            , const WChar * _windowName,	DWORD _style, int _x, int _y
            , int _width, int _height, HWND _parent, HMENU _hMenu
            , HINSTANCE _hInstance,	LPVOID _param ) override;

        void destroyWindow( HWND _hwnd ) override;
        LPVOID getCreateParams( LPARAM _lParam ) override;
        LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong ) override;
        LONG getWindowLong( HWND _hWnd,	int _index ) override;
        LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong ) override;
        LONG_PTR getWindowLongPtr( HWND _hWnd, int _index ) override;
        LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam ) override;
        
		void updateMessage( HWND _hWnd ) override;

        int messageBox( HWND _hWnd, const WChar * _text, const WChar * _caption, UINT _type ) override;

        bool getModuleFileName( HMODULE hModule, WChar * _moduleFilename, size_t _capacity ) override;
        //LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
        //LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName );

        bool makeFormatMessage( HRESULT _hresult, WString & _out ) override;
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _concatenatePath, size_t _capacity ) override;

		bool cmd( const WString & _command ) override;

		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;


	protected:
		bool validateFile_( const WChar * _path );

		bool createDirectoryUser_( const WChar * _userPath, const WString & _path, const WString & _file, const void * _data, size_t _size );

    protected:
        EWindowsType m_windowsType;

        bool m_checkedUnicode;
        bool m_supportUnicode;
    };
}