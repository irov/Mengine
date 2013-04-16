#   include "VistaWindowsLayer.h"

#   include "DynamicLibrary.h"

#   include <cassert>

#   include "Interface/UnicodeInterface.h"

#   include "Config/Typedef.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    VistaWindowsLayer::VistaWindowsLayer()
        : m_serviceProvider(NULL)
        , m_windowsType(EWT_UNKNOWN)
        , m_checkedUnicode(false)
        , m_supportUnicode(false)        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void VistaWindowsLayer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * VistaWindowsLayer::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterface * VistaWindowsLayer::loadDynamicLibrary( const WString & _path )
    {
        LOGGER_INFO(m_serviceProvider)("VistaWindowsLayer::loadDynamicLibrary load %ls"
            , _path.c_str()
            );

        DynamicLibrary * dl = new DynamicLibrary(_path);

        if( dl->load() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::loadDynamicLibrary invalid load %ls"
                , _path.c_str()
                );

            dl->destroy();

            return NULL;                 
        }

        return dl;
    }
    //////////////////////////////////////////////////////////////////////////
    EWindowsType VistaWindowsLayer::getWindowsType()
    {
        if( m_windowsType != EWT_UNKNOWN )
        {
            return m_windowsType;
        }

        m_windowsType = EWT_NT; 
        OSVERSIONINFOEX osvi;
        ::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        if( ::GetVersionEx((LPOSVERSIONINFO)&osvi) == 0)
        {
            osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
            if ( ::GetVersionEx((LPOSVERSIONINFO)&osvi) == 0)
            {
                return m_windowsType;
            }
        }

        if( osvi.dwMajorVersion >= 6 )
        {
            m_windowsType = EWT_VISTA;
        }
        else if( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // let's check Win95, 98, *AND* ME.
        {
            m_windowsType = EWT_98;
        }

        return m_windowsType;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::setProcessDPIAware()
    {
        HMODULE hUser32 = ::LoadLibrary( L"user32.dll" );

        if( hUser32 == NULL )
        {
            return false;
        }

        typedef BOOL (WINAPI *PSETPROCESSDPIAWARE)(VOID);

        PSETPROCESSDPIAWARE pSetProcessDPIAware = 
            ::GetProcAddress( hUser32, "SetProcessDPIAware" );

        if( pSetProcessDPIAware == NULL )
        {
            ::FreeLibrary( hUser32 );

            return false;
        }

        if( pSetProcessDPIAware() == FALSE )
        {
            ::FreeLibrary( hUser32 );

            return false;
        }

        ::FreeLibrary( hUser32 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::supportUnicode()
    {
        if( m_checkedUnicode == true )
        {
            return m_supportUnicode;
        }

        m_checkedUnicode = true;

        HMODULE hKernel32 = ::LoadLibrary( L"Kernel32.dll" );

        if( hKernel32 != NULL )
        {
            ::FreeLibrary( hKernel32 );

            m_supportUnicode = true;
        }

        return m_supportUnicode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::setCurrentDirectory( const WString & _path )
    {
        if( ::SetCurrentDirectory( _path.c_str() ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::setModuleCurrentDirectory()
    {
        WChar exeFilePath[MAX_PATH];
        ::GetModuleFileName( NULL, exeFilePath, MAX_PATH );

        WString unicode_exeFilePath(exeFilePath);
        WString::size_type slashPos = unicode_exeFilePath.find_last_of( L'\\' );

        WString unicode_exeFileDir = unicode_exeFilePath.substr( 0, slashPos );

        if( slashPos == WString::npos )
        {
            return false;
        }

        if( ::SetCurrentDirectory( unicode_exeFileDir.c_str() ) == FALSE )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::createDirectory( const WString& _path )
    {
        if( ::SHCreateDirectoryEx( NULL, _path.c_str(), NULL ) == ERROR_SUCCESS )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::fileExists( const WString& _path )
    {
        if( _path.empty() == true )	// current dir
        {
            return true;
        }

        WString::size_type length = _path.length();

        if( _path[length - 1] == L':' )	// root dir
        {
            return true;	// let it be
        }

        DWORD attributes = GetFileAttributes( _path.c_str() );

        if( attributes == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool getFileName( const WString & _filename, WString & _out )
    {
        WString::size_type pos1 = _filename.find_last_of(L"\\");

        WString::size_type pos2 = _filename.find_last_of(L"/");

        WString::size_type pos;

        if( pos1 != WString::npos && pos2 != WString::npos )
        {
            pos = (std::max)( pos1, pos2 );
        } 
        else if( pos1 == WString::npos )
        {
            pos = pos2;
        }
        else if( pos2 == WString::npos )
        {
            pos = pos1;
        }
        else
        {
            return false;
        }

        WString::size_type length = _filename.length();

        _out = _filename.substr( ++pos, length );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HANDLE VistaWindowsLayer::createFile( const WString & _path, DWORD _desiredAccess,
        DWORD _sharedMode, DWORD _creationDisposition )
    {
        HANDLE handle = ::CreateFile( _path.c_str(), _desiredAccess, _sharedMode, NULL,
            _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

#ifdef _DEBUG
        if( handle == INVALID_HANDLE_VALUE )
        {
            return INVALID_HANDLE_VALUE;
        }

        WIN32_FIND_DATA wfd;
        HANDLE hFind = ::FindFirstFile( _path.c_str(), &wfd );

        WString filename;
        getFileName( _path, filename );

        if( hFind == INVALID_HANDLE_VALUE )
        {
            printf("File invalid open ??? (%ls)"
                , filename.c_str()
                );
        }

        if( filename != wfd.cFileName )
        {
            printf("File invalid name lowercase|upcase:\npath - %ls\ncurrent file name - %ls\n\n"
                , filename.c_str()
                , wfd.cFileName
                );
        }
#endif

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    ATOM VistaWindowsLayer::registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
        , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
        , const WString& _className )
    {
        WNDCLASS wc;
        ::ZeroMemory( &wc, sizeof(WNDCLASS) );
        wc.cbClsExtra = _clsExtra;
        wc.cbWndExtra = _wndExtra;
        wc.style = /*CS_DBLCLKS |*/ CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = _wndProc;
        wc.hInstance = _hInstance;

        wc.hIcon = LoadIcon( _hInstance, MAKEINTRESOURCEW(_hIcon) );
        wc.hCursor = LoadCursor( NULL, MAKEINTRESOURCEW(32512) );

        wc.lpszClassName = _className.c_str();
        wc.hbrBackground = _hbrBackground;

        ATOM atom = ::RegisterClass( &wc );

        return atom;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::unregisterClass( const WString& _className, HINSTANCE _hInstance )
    {
        if( ::UnregisterClass( _className.c_str(), _hInstance ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND VistaWindowsLayer::createWindow( const WString& _className, const WString& _windowName
        , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
        , HINSTANCE _hInstance, LPVOID _param )
    {
        HWND hwnd = ::CreateWindow( _className.c_str(), _windowName.c_str()
            , _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND VistaWindowsLayer::createWindowEx( DWORD _exStyle, const WString& _className
        , const WString& _windowName,	DWORD _style, int _x, int _y
        , int _width, int _height, HWND _parent, HMENU _hMenu
        , HINSTANCE _hInstance,	LPVOID _param )
    {
        HWND hwnd = ::CreateWindowEx( _exStyle, _className.c_str(), _windowName.c_str()
            , _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

        //SetWindowTextW( hwnd, _windowName.c_str() );

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    void VistaWindowsLayer::destroyWindow( HWND _hwnd )
    {
        ::DestroyWindow( _hwnd );
    }
    //////////////////////////////////////////////////////////////////////////
    LPVOID VistaWindowsLayer::getCreateParams( LPARAM _lParam )
    {
        LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)_lParam;

        return createStruct->lpCreateParams;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG VistaWindowsLayer::setWindowLong( HWND _hWnd,	int _index,	LONG _newLong )
    {
        LONG value = ::SetWindowLong( _hWnd, _index, _newLong );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG VistaWindowsLayer::getWindowLong( HWND _hWnd,	int _index )
    {
        LONG value = ::GetWindowLong( _hWnd, _index );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG_PTR VistaWindowsLayer::setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong )
    {
        LONG_PTR value = ::SetWindowLongPtr( _hWnd, _index, _newLong );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG_PTR VistaWindowsLayer::getWindowLongPtr( HWND _hWnd, int _index )
    {
        LONG_PTR value = ::GetWindowLongPtr( _hWnd, _index );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT VistaWindowsLayer::defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
    {
        LRESULT result = ::DefWindowProc( _hWnd, _msg, _wParam, _lParam );

        return result;		
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::getCurrentDirectory( WString & _path )
    {
        WChar buffer[MAX_PATH];

        if( ::GetCurrentDirectory( MAX_PATH, buffer ) == 0 )
        {
            return false;
        }

        //WChar shortpath[MAX_PATH];
        //GetShortPathName( buffer, shortpath, MAX_PATH );

        _path.assign( buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax, UINT _removeMsg )
    {
        if( ::PeekMessage(  _msg, _hWnd, _msgFilterMin, _msgFilterMax, _removeMsg ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    LRESULT VistaWindowsLayer::dispatchMessage( const MSG* _msg )
    {
        LRESULT result = ::DispatchMessage( _msg );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    int VistaWindowsLayer::messageBox( HWND _hWnd, const WString& _text, const WString& _caption, UINT _type )
    {
        int result = ::MessageBox( _hWnd, _text.c_str(), _caption.c_str(), _type );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::getModuleFileName( HMODULE hModule, WString & _moduleFilename )
    {
        WChar exeFilePath[MAX_PATH];
        if( ::GetModuleFileName( hModule, exeFilePath, MAX_PATH ) == 0 )
        {
            return false;
        }

        _moduleFilename.assign( exeFilePath );

        return true;             
    }
    ////////////////////////////////////////////////////////////////////////////
    //LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData )
    //{
    //    HKEY openedKey; 
    //    LONG result;

    //    bool isStringValue = _dwType == REG_SZ || _dwType == REG_MULTI_SZ || _dwType == REG_EXPAND_SZ;

    //    /*Menge::String keyName;
    //    size_t separatorIndex = _lpValueName.find_last_of("\\");
    //    keyName = _lpValueName.substr(0, separatorIndex);
    //    _lpValueName = _lpValueName.substr(separatorIndex + 1);*/

    //    Menge::WString lpDataW;

    //    ::RegOpenKeyEx(_hKey, _lpKeyName.c_str(), 0, KEY_ALL_ACCESS, &openedKey);

    //    if( isStringValue == true )
    //    {
    //        Menge::String str( reinterpret_cast<const char*>( _lpData ) );
    //        utf8ToUnicode(str, lpDataW);
    //        _cbData = static_cast<DWORD>( (lpDataW.length()+1) * 2 );
    //        _lpData = reinterpret_cast<const BYTE*>( lpDataW.c_str() );
    //    }

    //    result = ::RegSetValueEx( _hKey, _lpValueName.c_str(), 0, _dwType, _lpData, _cbData );

    //    ::RegCloseKey( openedKey );

    //    return result;
    //}
    ////////////////////////////////////////////////////////////////////////////
    //LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName )
    //{
    //    HKEY openedKey;
    //    ::RegOpenKeyEx( _hKey, _lpKeyName.c_str(), 0, KEY_ALL_ACCESS, &openedKey );

    //    LONG result = ::RegDeleteValue( openedKey, _lpValueName.c_str() );

    //    ::RegCloseKey( openedKey );

    //    return result;
    //}
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::makeFormatMessage( HRESULT _hresult, WString & _out )
    {
        LPTSTR errorText = NULL;

        FormatMessage(
            // use system message tables to retrieve error text
            FORMAT_MESSAGE_FROM_SYSTEM
            // allocate buffer on local heap for error text
            |FORMAT_MESSAGE_ALLOCATE_BUFFER
            // Important! will fail otherwise, since we're not 
            // (and CANNOT) pass insertion parameters
            |FORMAT_MESSAGE_IGNORE_INSERTS,  
            NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
            _hresult,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errorText,  // output 
            0, // minimum size for output buffer
            NULL);   // arguments - see note 

        if( errorText == NULL )
        {
            return false;
        }

        _out.assign( errorText );

        LocalFree( errorText );

        return true;
    }
}