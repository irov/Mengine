#   include "VistaWindowsLayer.h"

#   include "DynamicLibrary.h"

#   include <cassert>

#   include "Interface/UnicodeInterface.h"

#   include "Config/Typedef.h"

#   include "Logger/Logger.h"

#   include <Shlwapi.h>

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
    bool VistaWindowsLayer::setCurrentDirectory( const WChar * _path )
    {
        if( ::SetCurrentDirectory( _path ) == FALSE )
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
    bool VistaWindowsLayer::createDirectory( const WChar * _path )
    {
        int cd_code = ::SHCreateDirectoryEx( NULL, _path, NULL );

        if( cd_code == ERROR_SUCCESS )
        {
            return true;
        }

        if( cd_code == ERROR_ALREADY_EXISTS )
        {
            LOGGER_WARNING(m_serviceProvider)("VistaWindowsLayer::createDirectory %ls alredy exists"
                , _path
                );

            return true;
        }
    
        DWORD err = GetLastError();

        WString message;
        this->makeFormatMessage( err, message );

        LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectory invalid create dir '%ls' - '%ls'"
            , _path
            , message.c_str()
            );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::fileExists( const WChar * _path )
    {
        size_t len = wcslen( _path );
        if( len == 0 )	// current dir
        {
            return true;
        }

        if( _path[len - 1] == L':' )	// root dir
        {
            return true;	// let it be
        }

        DWORD attributes = GetFileAttributes( _path );

        if( attributes == INVALID_FILE_ATTRIBUTES )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HANDLE VistaWindowsLayer::createFile( const WChar * _path, DWORD _desiredAccess,
        DWORD _sharedMode, DWORD _creationDisposition )
    {
        HANDLE handle = ::CreateFile( _path, _desiredAccess, _sharedMode, NULL,
            _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

#ifdef _DEBUG
        if( handle == INVALID_HANDLE_VALUE )
        {
            return INVALID_HANDLE_VALUE;
        }

        WIN32_FIND_DATA wfd;
        HANDLE hFind = ::FindFirstFile( _path, &wfd );

        if( hFind == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR(m_serviceProvider)("File invalid find ??? (%ls)\n"
                , _path
                );
        }

        const WChar * filename = PathFindFileName( _path );

        if( wcscmp( filename, wfd.cFileName ) != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("File invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                , _path
                , filename
                , wfd.cFileName
                );

            return INVALID_HANDLE_VALUE;
        }
#endif

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    ATOM VistaWindowsLayer::registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
        , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
        , const WChar * _className )
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

        wc.lpszClassName = _className;
        wc.hbrBackground = _hbrBackground;

        ATOM atom = ::RegisterClass( &wc );

        return atom;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::unregisterClass( const WChar * _className, HINSTANCE _hInstance )
    {
        if( ::UnregisterClass( _className, _hInstance ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND VistaWindowsLayer::createWindow( const WChar * _className, const WChar * _windowName
        , DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
        , HINSTANCE _hInstance, LPVOID _param )
    {
        HWND hwnd = ::CreateWindow( _className, _windowName
            , _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    HWND VistaWindowsLayer::createWindowEx( DWORD _exStyle, const WChar * _className
        , const WChar * _windowName,	DWORD _style, int _x, int _y
        , int _width, int _height, HWND _parent, HMENU _hMenu
        , HINSTANCE _hInstance,	LPVOID _param )
    {
        HWND hwnd = ::CreateWindowEx( _exStyle, _className, _windowName
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
    int VistaWindowsLayer::messageBox( HWND _hWnd, const WChar * _text, const WChar * _caption, UINT _type )
    {
        int result = ::MessageBox( _hWnd, _text, _caption, _type );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::getModuleFileName( HMODULE hModule, WChar * _moduleFilename, size_t _capacity )
    {
        if( ::GetModuleFileName( hModule, _moduleFilename, _capacity ) == 0 )
        {
            return false;
        }

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
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::concatenateFilePath( const FilePath & _folder, const FilePath & _filename, WChar * _filePath, size_t _capacity )
    {        
        size_t folderSize = _folder.size();
        size_t fileNameSize = _filename.size();
                        
        size_t filePathSize = folderSize + fileNameSize;
        
        if( filePathSize >= MAX_PATH )
        {
            return false;
        }

        Char filePath[MAX_PATH];
        memcpy(filePath, _folder.c_str(), folderSize);        
        memcpy(filePath + folderSize, _filename.c_str(), fileNameSize );

        filePath[filePathSize] = L'\0';
        filePathSize += 1; //Null

        if( UNICODE_SERVICE(m_serviceProvider)
            ->utf8ToUnicode( filePath, filePathSize, _filePath, _capacity, nullptr ) == false )
        {
            return false;
        }
        
        return true;
    }
}