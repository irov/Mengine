#   include "VistaWindowsLayer.h"

#   include "DynamicLibrary.h"

//#   include <cassert>

#   include "Interface/UnicodeInterface.h"

#   include "Config/Typedef.h"

#   include "Logger/Logger.h"

#   include <Shlwapi.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void PathCorrectBackslash( WChar * _out, const WChar * _in )
	{
		wcscpy( _out, _in );

		WChar * pch = wcschr( _out, '/' );
		while( pch != NULL )
		{
			*pch = '\\';

			pch = wcschr( pch + 1, '/' );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    VistaWindowsLayer::VistaWindowsLayer()
        : m_serviceProvider(nullptr)
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

        DynamicLibrary * dl = new DynamicLibrary(m_serviceProvider, _path);

        if( dl->load() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::loadDynamicLibrary invalid load %ls"
                , _path.c_str()
                );

            dl->destroy();

            return nullptr;                 
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

        OSVERSIONINFO osvi;
        ::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));

        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if( ::GetVersionEx((LPOSVERSIONINFO)&osvi) == TRUE )
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
		WChar pathCorrect[MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );

        if( ::SetCurrentDirectory( pathCorrect ) == FALSE )
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
		WChar pathCorrect[MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );
		
		PathRemoveBackslash( pathCorrect );

		if( PathIsDirectoryW( pathCorrect ) == FILE_ATTRIBUTE_DIRECTORY )
		{
			return true;
		}

		TVectorWString paths;

		while( true, true )
		{
			paths.push_back( pathCorrect );

			if( PathRemoveFileSpecW( pathCorrect ) == FALSE )
			{
				break;
			}

			if( PathIsDirectoryW( pathCorrect ) == FILE_ATTRIBUTE_DIRECTORY )
			{
				break;
			}
		}

		for( TVectorWString::reverse_iterator
			it = paths.rbegin(),
			it_end = paths.rend();
		it != it_end;
		++it )
		{
			const WString & path = *it;

			BOOL successful = CreateDirectory( path.c_str(), NULL );

			if( successful == FALSE )
			{
				DWORD err = GetLastError();

				switch( err )
				{
				case ERROR_ALREADY_EXISTS:
					{
						LOGGER_WARNING(m_serviceProvider)("VistaWindowsLayer::createDirectory %ls alredy exists"
							, path.c_str()
							);

						return false;
					}break;
				case ERROR_PATH_NOT_FOUND:
					{
						LOGGER_WARNING(m_serviceProvider)("VistaWindowsLayer::createDirectory %ls not found"
							, path.c_str()
							);

						return false;
					}break;
				default:
					{
						LOGGER_WARNING(m_serviceProvider)("VistaWindowsLayer::createDirectory %ls unknown error %d"
							, path.c_str()
							, err
							);

						return false;
					}break;
				}
			}
		}

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VistaWindowsLayer::fileExists( const WChar * _path )
    {
		WChar pathCorrect[MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );

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
		WChar pathCorrect[MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );

        HANDLE handle = ::CreateFile( pathCorrect, _desiredAccess, _sharedMode, NULL,
            _creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

		if( handle == INVALID_HANDLE_VALUE )
		{
			DWORD err_code = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createFile invalid create file %ls err %d"
				, pathCorrect
				, err_code
				);

			return INVALID_HANDLE_VALUE;
		}

#ifdef _DEBUG
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
			LOGGER_ERROR(m_serviceProvider)("File invalid find ??? (%ls)\n"
				, _path
				);
		}

		const WChar * filename = PathFindFileName( _path );

		//WCHAR finalPath[MAX_PATH];
		//GetFinalPathNameByHandle( handle, finalPath, MAX_PATH, VOLUME_NAME_DOS );

		//WCHAR canonicalizePath[MAX_PATH];
		//PathCanonicalize( canonicalizePath, finalPath );

		//WCHAR currentDirectory[MAX_PATH];
		//GetCurrentDirectory( MAX_PATH, currentDirectory );

		//WCHAR relativePath[MAX_PATH];
		//if( PathRelativePathTo( relativePath, currentDirectory, FILE_ATTRIBUTE_DIRECTORY, canonicalizePath, FILE_ATTRIBUTE_NORMAL ) == FALSE )
		//{
		//    LOGGER_ERROR(m_serviceProvider)("File invalid relative path\nfrom: %ls\nto: %ls"
		//        , currentDirectory
		//        , canonicalizePath
		//        );
		//    
		//    ::CloseHandle( handle );

		//    return INVALID_HANDLE_VALUE;
		//}

		if( wcscmp( filename, wfd.cFileName ) != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("File invalid name lowercase|upcase:\npath - '%ls'\nneed file name - '%ls'\ncurrent file name - '%ls'\n\n"
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
        _path += MENGE_FOLDER_DELIM;

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
    bool VistaWindowsLayer::concatenateFilePath( const ConstString & _folder, const ConstString & _fileName, WChar * _filePath, size_t _capacity )
    {        
        size_t folderSize = _folder.size();
		size_t dirSize = _fileName.size();
                        
        size_t filePathSize = folderSize + dirSize;
        
        if( filePathSize >= MAX_PATH )
        {
            return false;
        }

        Char filePath[MAX_PATH];
        stdex::memorycopy(filePath, 0, _folder.c_str(), folderSize);
		stdex::memorycopy(filePath, folderSize, _fileName.c_str(), dirSize);

        filePath[filePathSize] = L'\0';
        filePathSize += 1; //Null

		WChar filePathW[MAX_PATH];
        if( UNICODE_SERVICE(m_serviceProvider)
            ->utf8ToUnicode( filePath, filePathSize, filePathW, _capacity, nullptr ) == false )
        {
            return false;
        }

		PathCorrectBackslash( _filePath, filePathW );
        
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool VistaWindowsLayer::cmd( const WString & _command )
	{
		const wchar_t * wc = _command.c_str();

		int err = _wsystem( wc );

		if( err != 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::cmd: command:\n%ls\nerror: %d"
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
		WChar szPath[MAX_PATH] = {0};

		PathAppend( szPath, _userPath );

		WChar pathCorrect[MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path.c_str() );

		WChar fileCorrect[MAX_PATH];
		PathCorrectBackslash( fileCorrect, _file.c_str() );

		PathAppend( szPath, pathCorrect );

		if( this->fileExists( szPath ) == false )
		{
			if( this->createDirectory( szPath ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid createDirectory %s"
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
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid createFile %s"
				, pathCorrect
				, fileCorrect
				, szPath
				);

			return false;
		}

		DWORD bytesWritten = 0;
		BOOL result = ::WriteFile( hFile, _data, _size, &bytesWritten, NULL );

		::CloseHandle( hFile );

		if( result == FALSE )
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserPicture: %ls:%ls invalid writeFile %s"
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
		WCHAR szPath[MAX_PATH];

		if(FAILED(SHGetFolderPath(NULL
			, CSIDL_COMMON_PICTURES | CSIDL_FLAG_CREATE
			, NULL
			, 0
			, szPath))) 
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserPicture: '%ls:%ls' invalid SHGetFolderPath CSIDL_COMMON_PICTURES"
				, _path.c_str()
				, _file.c_str()
				);

			return false;
		}

		if( this->createDirectoryUser_( szPath, _path, _file, _data, _size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserPicture: '%ls:%ls' invalid createDirectoryUser_ '%ls'"
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
		WCHAR szPath[MAX_PATH];

		if(FAILED(SHGetFolderPath(NULL
			, CSIDL_COMMON_MUSIC | CSIDL_FLAG_CREATE
			, NULL
			, 0
			, szPath))) 
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserMusic: %ls:%ls invalid SHGetFolderPath CSIDL_COMMON_MUSIC"
				, _path.c_str()
				, _file.c_str()
				);

			return false;
		}	

		if( this->createDirectoryUser_( szPath, _path, _file, _data, _size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("VistaWindowsLayer::createDirectoryUserMusic: '%ls:%ls' invalid createDirectoryUser_ '%ls'"
				, _path.c_str()
				, _file.c_str()
				, szPath
				);

			return false;
		}

		return true;
	}
}