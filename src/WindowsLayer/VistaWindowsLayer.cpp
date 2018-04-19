#include "VistaWindowsLayer.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/PlatformInterface.h"

#include "Config/Typedef.h"

#include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WindowsLayer, Mengine::VistaWindowsLayer );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VistaWindowsLayer::VistaWindowsLayer()
        : m_windowsType( EWT_UNKNOWN )
        , m_checkedUnicode( false )
        , m_supportUnicode( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EWindowsType VistaWindowsLayer::getWindowsType()
    {
        if( m_windowsType != EWT_UNKNOWN )
        {
            return m_windowsType;
        }

        OSVERSIONINFO osvi;
        ::ZeroMemory( &osvi, sizeof( OSVERSIONINFO ) );

        osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
        if( ::GetVersionEx( (LPOSVERSIONINFO)&osvi ) == TRUE )
        {
            if( osvi.dwMajorVersion >= 6 )
            {
                m_windowsType = EWT_VISTA;
            }
            else if( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // let's check Win95, 98, *AND* ME.
            {
                m_windowsType = EWT_98;
            }
        }
        else
        {
            m_windowsType = EWT_NT;
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

        typedef BOOL( WINAPI *PSETPROCESSDPIAWARE )(VOID);

        FARPROC pSetProcessDPIAware =
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
    bool VistaWindowsLayer::fileExists( const WChar * _path )
    {
        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( pathCorrect, _path );

        size_t len = wcslen( pathCorrect );
        if( len == 0 )	// current dir
        {
            return true;
        }

        if( pathCorrect[len - 1] == L':' )	// root dir
        {
            return true;	// let it be
        }

        DWORD attributes = GetFileAttributes( pathCorrect );

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
        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( pathCorrect, _path );

        HANDLE handle = ::CreateFile( pathCorrect, _desiredAccess, _sharedMode, NULL,
            _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

        if( handle == INVALID_HANDLE_VALUE )
        {
            DWORD err_code = GetLastError();

            LOGGER_ERROR("VistaWindowsLayer::createFile invalid create file %ls err %d"
                , pathCorrect
                , err_code
                );

            return INVALID_HANDLE_VALUE;
        }

#ifndef NDEBUG
        if( this->validateFile_( pathCorrect ) == false )
        {
            ::CloseHandle( handle );

            return INVALID_HANDLE_VALUE;
        }
#endif

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::validateFile_( const WChar * _path )
    {
        WIN32_FIND_DATA wfd;
        HANDLE hFind = ::FindFirstFile( _path, &wfd );

        if( hFind == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR("File invalid find ??? (%ls)\n"
                , _path
                );

            return false;
        }

        const WChar * filename = PathFindFileName( _path );

        if( wcscmp( filename, wfd.cFileName ) != 0 )
        {
            LOGGER_ERROR("File invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
                , _path
                , filename
                , wfd.cFileName
                );

            ::FindClose( hFind );

            return false;
        }

        ::FindClose( hFind );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ATOM VistaWindowsLayer::registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
        , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
        , const WChar * _className )
    {
        WNDCLASS wc;
        ::ZeroMemory( &wc, sizeof( WNDCLASS ) );
        wc.cbClsExtra = _clsExtra;
        wc.cbWndExtra = _wndExtra;
        wc.style = /*CS_DBLCLKS |*/ CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = _wndProc;
        wc.hInstance = _hInstance;

        wc.hIcon = LoadIcon( _hInstance, MAKEINTRESOURCEW( _hIcon ) );
        wc.hCursor = LoadCursor( NULL, MAKEINTRESOURCEW( 32512 ) );

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
        , const WChar * _windowName, DWORD _style, int _x, int _y
        , int _width, int _height, HWND _parent, HMENU _hMenu
        , HINSTANCE _hInstance, LPVOID _param )
    {
        HWND hwnd = ::CreateWindowEx( _exStyle, _className, _windowName
            , _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

        //SetWindowTextW( hwnd, _windowName.c_str() );

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    void VistaWindowsLayer::destroyWindow( HWND _hWnd )
    {
        ::CloseWindow( _hWnd );
        ::DestroyWindow( _hWnd );
    }
    //////////////////////////////////////////////////////////////////////////
    LPVOID VistaWindowsLayer::getCreateParams( LPARAM _lParam )
    {
        LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)_lParam;

        return createStruct->lpCreateParams;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG VistaWindowsLayer::setWindowLong( HWND _hWnd, int _index, LONG _newLong )
    {
        LONG value = ::SetWindowLong( _hWnd, _index, _newLong );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    LONG VistaWindowsLayer::getWindowLong( HWND _hWnd, int _index )
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
    void VistaWindowsLayer::updateMessage( HWND _hWnd )
    {
        MSG  msg;
        while( ::PeekMessage( &msg, _hWnd, 0U, 0U, PM_REMOVE ) == TRUE )
        {
            ::TranslateMessage( &msg );

            ::DispatchMessage( &msg );
        }
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
        if( ::GetModuleFileName( hModule, _moduleFilename, (DWORD)_capacity ) == 0 )
        {
            return false;
        }

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    //LONG setRegistryValue( HKEY _hKey, const Mengine::WString & _lpKeyName, const Mengine::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData )
    //{
    //    HKEY openedKey; 
    //    LONG result;

    //    bool isStringValue = _dwType == REG_SZ || _dwType == REG_MULTI_SZ || _dwType == REG_EXPAND_SZ;

    //    /*Mengine::String keyName;
    //    size_t separatorIndex = _lpValueName.find_last_of("\\");
    //    keyName = _lpValueName.substr(0, separatorIndex);
    //    _lpValueName = _lpValueName.substr(separatorIndex + 1);*/

    //    Mengine::WString lpDataW;

    //    ::RegOpenKeyEx(_hKey, _lpKeyName.c_str(), 0, KEY_ALL_ACCESS, &openedKey);

    //    if( isStringValue == true )
    //    {
    //        Mengine::String str( reinterpret_cast<const char*>( _lpData ) );
    //        utf8ToUnicode(str, lpDataW);
    //        _cbData = static_cast<DWORD>( (lpDataW.length()+1) * 2 );
    //        _lpData = reinterpret_cast<const BYTE*>( lpDataW.c_str() );
    //    }

    //    result = ::RegSetValueEx( _hKey, _lpValueName.c_str(), 0, _dwType, _lpData, _cbData );

    //    ::RegCloseKey( openedKey );

    //    return result;
    //}
    ////////////////////////////////////////////////////////////////////////////
    //LONG deleteRegistryValue( HKEY _hKey, const Mengine::WString & _lpKeyName, const Mengine::WString & _lpValueName )
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

        if( FormatMessage(
            // use system message tables to retrieve error text
            FORMAT_MESSAGE_FROM_SYSTEM
            // allocate buffer on local heap for error text
            | FORMAT_MESSAGE_ALLOCATE_BUFFER
            // Important! will fail otherwise, since we're not 
            // (and CANNOT) pass insertion parameters
            | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
            _hresult,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            (LPTSTR)&errorText,  // output 
            0, // minimum size for output buffer
            NULL ) == 0 )
        {


            return false;
        }

        _out.assign( errorText );

        LocalFree( errorText );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _concatenatePath, size_t _capacity )
    {
        size_t relationSize = _relationPath.size();
        size_t folderSize = _folderPath.size();        
        size_t fileSize = _filePath.size();

        size_t filePathSize = relationSize + folderSize + fileSize;

        if( filePathSize >= MENGINE_MAX_PATH )
        {
            return false;
        }

        Char filePath[MENGINE_MAX_PATH];
        stdex::memorycopy( filePath, 0, _relationPath.c_str(), relationSize );
        stdex::memorycopy( filePath, relationSize, _folderPath.c_str(), folderSize );
        stdex::memorycopy( filePath, relationSize + folderSize, _filePath.c_str(), fileSize );

        filePath[filePathSize] = '\0';
        filePathSize += 1; //Null

        WChar filePathW[MENGINE_MAX_PATH];
        if( UNICODE_SYSTEM()
            ->utf8ToUnicode( filePath, filePathSize, filePathW, _capacity, nullptr ) == false )
        {
            return false;
        }

        Helper::pathCorrectBackslash( _concatenatePath, filePathW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::cmd( const WString & _command )
    {
        const wchar_t * wc = _command.c_str();

        int err = _wsystem( wc );

        if( err != 0 )
        {
            LOGGER_ERROR("VistaWindowsLayer::cmd: command:\n%ls\nerror: %d"
                , _command.c_str()
                , errno
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::createDirectoryUser_( const WChar * _userPath, const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        WChar szPath[MENGINE_MAX_PATH] = { 0 };

        PathAppend( szPath, _userPath );

        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( pathCorrect, _path.c_str() );

        WChar fileCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( fileCorrect, _file.c_str() );

        PathAppend( szPath, pathCorrect );

        if( this->fileExists( szPath ) == false )
        {
            if( PLATFORM_SERVICE()
                ->createDirectory( szPath ) == false )
            {
                LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid createDirectory %s"
                    , pathCorrect
                    , fileCorrect
                    , szPath
                    );

                return false;
            }
        }

        PathAppend( szPath, fileCorrect );

        HANDLE hFile = this->createFile( szPath
            , GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , CREATE_ALWAYS
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid createFile %s"
                , pathCorrect
                , fileCorrect
                , szPath
                );

            return false;
        }

        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( hFile, _data, (DWORD)_size, &bytesWritten, NULL );

        ::CloseHandle( hFile );

        if( result == FALSE )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid writeFile %s"
                , pathCorrect
                , fileCorrect
                , szPath
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        WCHAR szPath[MENGINE_MAX_PATH];

        if( FAILED( SHGetFolderPath( NULL
            , CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserPicture: '%ls:%ls' invalid SHGetFolderPath CSIDL_COMMON_PICTURES"
                , _path.c_str()
                , _file.c_str()
                );

            return false;
        }

        if( this->createDirectoryUser_( szPath, _path, _file, _data, _size ) == false )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserPicture: '%ls:%ls' invalid createDirectoryUser_ '%ls'"
                , _path.c_str()
                , _file.c_str()
                , szPath
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        WCHAR szPath[MENGINE_MAX_PATH];

        if( FAILED( SHGetFolderPath( NULL
            , CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE
            , NULL
            , 0
            , szPath ) ) )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserMusic: %ls:%ls invalid SHGetFolderPath CSIDL_COMMON_MUSIC"
                , _path.c_str()
                , _file.c_str()
                );

            return false;
        }

        if( this->createDirectoryUser_( szPath, _path, _file, _data, _size ) == false )
        {
            LOGGER_ERROR("VistaWindowsLayer::createDirectoryUserMusic: '%ls:%ls' invalid createDirectoryUser_ '%ls'"
                , _path.c_str()
                , _file.c_str()
                , szPath
                );

            return false;
        }

        return true;
    }
}