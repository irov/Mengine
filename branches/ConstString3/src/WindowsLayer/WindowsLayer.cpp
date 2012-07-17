/*
 *	WindowsLayer.cpp
 *
 *	Created by _Berserk_ on 10.3.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#include "WindowsLayer.h"

#include <cassert>

namespace WindowsLayer
{
	//////////////////////////////////////////////////////////////////////////
	EWindowsType getWindowsType()
	{
		static EWindowsType s_windowsType = EWT_UNKNOWN;
		if( s_windowsType != EWT_UNKNOWN )
		{
			return s_windowsType;
		}

		s_windowsType = EWT_NT; 
		OSVERSIONINFOEX osvi;
		::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if( ::GetVersionEx((LPOSVERSIONINFO)&osvi) == 0)
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if ( ::GetVersionEx((LPOSVERSIONINFO)&osvi) == 0)
			{
				return s_windowsType;
			}
		}

		if( osvi.dwMajorVersion >= 6 )
		{
			s_windowsType = EWT_VISTA;
		}
		else if( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // let's check Win95, 98, *AND* ME.
		{
			s_windowsType = EWT_98;
		}

		return s_windowsType;
	}
	//////////////////////////////////////////////////////////////////////////
	void setProcessDPIAware()
	{
		HMODULE hUser32 = ::LoadLibrary( L"user32.dll" );

		if( hUser32 == NULL )
		{
			return;
		}

		typedef BOOL (WINAPI *PSETPROCESSDPIAWARE)(VOID);
		
		PSETPROCESSDPIAWARE pSetProcessDPIAware = 
			::GetProcAddress( hUser32, "SetProcessDPIAware" );

		if( pSetProcessDPIAware != NULL )
		{
			pSetProcessDPIAware();
		}

		::FreeLibrary( hUser32 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool supportUnicode()
	{
		static bool s_checked = false;
		static bool s_result = false;

		if( s_checked == true )
		{
			return s_result;
		}

		HMODULE hKernel32 = ::LoadLibrary( L"Kernel32.dll" );
		if( hKernel32 != NULL )
		{
			::FreeLibrary( hKernel32 );
			s_result = true;
		}

		s_checked = true;

		return s_result;
	}
	//////////////////////////////////////////////////////////////////////////
	void utf8ToUnicode( const Menge::String& _utf8, Menge::WString & _wstr )
	{
		if( _utf8.empty() == true )
		{
			_wstr.clear();
			return;
		}

		const char* cutf8 = _utf8.c_str();
		int size = ::MultiByteToWideChar( CP_UTF8, 0, cutf8, -1, 0, 0 );

		//static WString s_buffer;
		_wstr.resize(size - 1);
		//wchar_t * buffer = new wchar_t[size];
		::MultiByteToWideChar( CP_UTF8, 0, cutf8, -1, &_wstr[0], size - 1 );
		//_wstr.assign( buffer );
		//delete [] buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void unicodeToUtf8( const Menge::WString& _wstr, Menge::String & _utf8 )
	{
		if( _wstr.empty() == true )
		{
			_utf8.clear();
			return;
		}

		const wchar_t* cwstr = _wstr.c_str();
		int size = ::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, 0, 0, 0, 0 );

		_utf8.resize(size - 1);
		//char * buffer = new char[size];
		::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, &_utf8[0], size - 1, NULL, NULL );
		//_utf8.assign( buffer );
		//delete [] buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void ansiToUnicode( const Menge::String& _ansi, Menge::WString & _wstr )
	{
		if( _ansi.empty() == true )
		{
			_wstr.clear();
			return;
		}

		const char* cansi = _ansi.c_str();
		int size = ::MultiByteToWideChar( CP_ACP, 0, cansi, -1, 0, 0 );

		_wstr.resize(size - 1);
		//wchar_t * buffer = new wchar_t[size];
		::MultiByteToWideChar( CP_ACP, 0, cansi, -1, &_wstr[0], size - 1 );
		//_wstr.assign( buffer );
		//delete [] buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void unicodeToAnsi( const Menge::WString& _wstr, Menge::String & _ansi )
	{
		if( _wstr.empty() == true )
		{
			_ansi.clear();
			return;
		}

		const wchar_t* cwstr = _wstr.c_str();
		int size = ::WideCharToMultiByte( CP_ACP, 0, cwstr, -1, 0, 0, 0, 0 );
		
		_ansi.resize( size - 1 );
		//char * buffer = new char[size];
		::WideCharToMultiByte( CP_ACP, 0, cwstr, -1, &_ansi[0], size - 1, NULL, NULL );
		//_ansi.assign( buffer );
		//delete [] buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void utf8ToAnsi( const Menge::String& _utf8, Menge::String & _ansi )
	{
		Menge::WString wstr;
		utf8ToUnicode( _utf8, wstr );
		unicodeToAnsi( wstr, _ansi );
	}
	//////////////////////////////////////////////////////////////////////////
	void ansiToUtf8( const Menge::String& _ansi, Menge::String & _utf8 )
	{
		Menge::WString wstr;
		ansiToUnicode( _ansi, wstr );
		unicodeToUtf8( wstr, _utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	void utf8Count( const Menge::String& _utf8, size_t & _size )
	{
		Menge::WString wstr;
		utf8ToUnicode( _utf8, wstr );
		//wstrToAnsi( wstr, _ansi );

		_size = wstr.size();
	}
	//////////////////////////////////////////////////////////////////////////
	bool setCurrentDirectory( const Menge::WString& _path )
	{
		BOOL result = FALSE;

		result = ::SetCurrentDirectory( _path.c_str() );
		
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool setModuleCurrentDirectory()
	{
		BOOL result = FALSE;

		wchar_t exeFilePath[MAX_PATH];
		::GetModuleFileName( NULL, exeFilePath, MAX_PATH );
		
		Menge::WString exeFileDir( exeFilePath );
		Menge::WString::size_type slashPos = exeFileDir.find_last_of( L'\\' );
		
		exeFileDir = exeFileDir.substr( 0, slashPos );
		
		if( slashPos != Menge::WString::npos )
		{
			result = ::SetCurrentDirectory( exeFileDir.c_str() );
		}
		
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool createDirectory( const Menge::WString& _path )
	{
		BOOL result = ::CreateDirectory( _path.c_str(), NULL );
		
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool fileExists( const Menge::WString& _path )
	{
		DWORD attributes = GetFileAttributes( _path.c_str() );

		if( attributes == INVALID_FILE_ATTRIBUTES )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static Menge::WString GetNameFromPath( Menge::WString sPath ) 
	{
		int pos1 = sPath.find_last_of(L"\\");
		
		int pos2 = sPath.find_last_of(L"/");

		int pos = max(pos1, pos2);

		return sPath.substr(++pos, sPath.length());
	}
	//////////////////////////////////////////////////////////////////////////
	HANDLE createFile( const Menge::WString& _filepath, DWORD _desiredAccess,
		DWORD _sharedMode, DWORD _creationDisposition )
	{
		HANDLE handle = ::CreateFile( _filepath.c_str(), _desiredAccess, _sharedMode, NULL,
			_creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );

#ifdef _DEBUG
		WIN32_FIND_DATA wfd;
		HANDLE hFind = ::FindFirstFile(_filepath.c_str(), &wfd);

		Menge::WString filename = GetNameFromPath(_filepath);

		if( filename != wfd.cFileName )
		{
			printf("File invalid name lowercase|upcase %S (%S)\n"
				, _filepath.c_str()
				, wfd.cFileName
				);
		}
#endif
		
		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
		, HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
		, const Menge::WString& _className )
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

		return ::RegisterClass( &wc );
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL unregisterClass( const Menge::WString& _className, HINSTANCE _hInstance )
	{
		BOOL result = ::UnregisterClass( _className.c_str(), _hInstance );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	HWND createWindow( const Menge::WString& _className, const Menge::WString& _windowName
		, DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance, LPVOID _param )
	{
		HWND hwnd = ::CreateWindow( _className.c_str(), _windowName.c_str()
			, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

		return hwnd;
	}
	//////////////////////////////////////////////////////////////////////////
	HWND createWindowEx( DWORD _exStyle, const Menge::WString& _className
		, const Menge::WString& _windowName,	DWORD _style, int _x, int _y
		, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance,	LPVOID _param )
	{
		HWND hwnd = ::CreateWindowEx( _exStyle, _className.c_str(), _windowName.c_str()
			, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );

		//SetWindowTextW( hwnd, _windowName.c_str() );

		return hwnd;
	}
	//////////////////////////////////////////////////////////////////////////
	void destroyWindow( HWND _hwnd )
	{
		::DestroyWindow( _hwnd );
	}
	//////////////////////////////////////////////////////////////////////////
	LPVOID getCreateParams( LPARAM _lParam )
	{
		LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)_lParam;

		return createStruct->lpCreateParams;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong )
	{
		LONG value = ::SetWindowLong( _hWnd, _index, _newLong );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG getWindowLong( HWND _hWnd,	int _index )
	{
		LONG value = ::GetWindowLong( _hWnd, _index );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong )
	{
		LONG_PTR value = ::SetWindowLongPtr( _hWnd, _index, _newLong );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG_PTR getWindowLongPtr( HWND _hWnd, int _index )
	{
		LONG_PTR value = ::GetWindowLongPtr( _hWnd, _index );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
	{
		LRESULT result = ::DefWindowProc( _hWnd, _msg, _wParam, _lParam );

		return result;		
	}
	//////////////////////////////////////////////////////////////////////////
	void getCurrentDirectory( Menge::WString & _path )
	{
		wchar_t buffer[MAX_PATH];
		::GetCurrentDirectory( MAX_PATH, buffer );		
		
		_path.assign( buffer );
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax
		, UINT _removeMsg )
	{
		BOOL result = ::PeekMessage(  _msg, _hWnd, _msgFilterMin, _msgFilterMax, _removeMsg );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT dispatchMessage( const MSG* _msg )
	{
		LRESULT result = ::DispatchMessage( _msg );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int messageBox( HWND _hWnd, const Menge::WString& _text, const Menge::WString& _caption, UINT _type )
	{
		int result = ::MessageBox( _hWnd, _text.c_str(), _caption.c_str(), _type );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void getModuleFileName( HMODULE hModule, Menge::WString & _moduleFilename )
	{
		wchar_t exeFilePath[MAX_PATH];
		::GetModuleFileName( hModule, exeFilePath, MAX_PATH );

		_moduleFilename.assign( exeFilePath );
	}
	//////////////////////////////////////////////////////////////////////////
	LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData )
	{
		HKEY openedKey; 
		LONG result;

		bool isStringValue = _dwType == REG_SZ || _dwType == REG_MULTI_SZ || _dwType == REG_EXPAND_SZ;
		
		/*Menge::String keyName;
		size_t separatorIndex = _lpValueName.find_last_of("\\");
		keyName = _lpValueName.substr(0, separatorIndex);
		_lpValueName = _lpValueName.substr(separatorIndex + 1);*/

		Menge::WString lpDataW;

		::RegOpenKeyEx(_hKey, _lpKeyName.c_str(), 0, KEY_ALL_ACCESS, &openedKey);

		if( isStringValue == true )
		{
			Menge::String str( reinterpret_cast<const char*>( _lpData ) );
			utf8ToUnicode(str, lpDataW);
			_cbData = static_cast<DWORD>( (lpDataW.length()+1) * 2 );
			_lpData = reinterpret_cast<const BYTE*>( lpDataW.c_str() );
		}

		result = ::RegSetValueEx( _hKey, _lpValueName.c_str(), 0, _dwType, _lpData, _cbData );
		
		::RegCloseKey( openedKey );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName )
	{
		HKEY openedKey;
		::RegOpenKeyEx( _hKey, _lpKeyName.c_str(), 0, KEY_ALL_ACCESS, &openedKey );

		LONG result = ::RegDeleteValue( openedKey, _lpValueName.c_str() );
		
		::RegCloseKey( openedKey );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool getShortPathName( const Menge::WString& _longPathName, Menge::WString & _shortPathName )
	{
		wchar_t shortPathNameW[MAX_PATH];
		if( ::GetShortPathName( _longPathName.c_str(), shortPathNameW, MAX_PATH ) == 0 )
		{
			return false;
		}

		_shortPathName.assign( shortPathNameW );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace WindowsLayer
