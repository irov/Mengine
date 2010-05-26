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
		OSVERSIONINFOEXA osvi;
		::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
		if( ::GetVersionExA((LPOSVERSIONINFOA)&osvi) == 0)
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOA);
			if ( ::GetVersionExA((LPOSVERSIONINFOA)&osvi) == 0)
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
		typedef BOOL (WINAPI *PSETPROCESSDPIAWARE)(VOID);

		HMODULE hUser32 = ::LoadLibraryA( "user32.dll" );
		if( hUser32 != NULL )
		{
			PSETPROCESSDPIAWARE pSetProcessDPIAware = 
				::GetProcAddress( hUser32, "SetProcessDPIAware" );
			if( pSetProcessDPIAware != NULL )
			{
				pSetProcessDPIAware();
			}
			::FreeLibrary( hUser32 );
		}
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

		HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );
		if( hKernel32 != NULL )
		{
			::FreeLibrary( hKernel32 );
			s_result = true;
		}
		s_checked = true;
		return s_result;
	}
	//////////////////////////////////////////////////////////////////////////
	void utf8ToWstr( const Menge::String& _utf8, Menge::StringW* _wstr )
	{
		assert( _wstr != NULL );
		const char* cutf8 = _utf8.c_str();
		int size = ::MultiByteToWideChar( CP_UTF8, 0, cutf8, -1, 0, 0 );
		std::vector<wchar_t> buffer( size );
		::MultiByteToWideChar( CP_UTF8, 0, cutf8, -1, &(buffer[0]), size );
		_wstr->assign( &(buffer[0]) );
	}
	//////////////////////////////////////////////////////////////////////////
	void wstrToUtf8( const Menge::StringW& _wstr, Menge::String* _utf8 )
	{
		assert( _utf8 != NULL );
		const wchar_t* cwstr = _wstr.c_str();
		int size = ::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, 0, 0, 0, 0 );
		std::vector<char> buffer(size);
		::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, &(buffer[0]), size, NULL, NULL );
		_utf8->assign( &(buffer[0]) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ansiToWstr( const Menge::String& _ansi, Menge::StringW* _wstr )
	{
		assert( _wstr != NULL );
		const char* cansi = _ansi.c_str();
		int size = ::MultiByteToWideChar( CP_ACP, 0, cansi, -1, 0, 0 );
		std::vector<wchar_t> buffer( size );
		::MultiByteToWideChar( CP_ACP, 0, cansi, -1, &(buffer[0]), size );
		_wstr->assign( &(buffer[0]) );
	}
	//////////////////////////////////////////////////////////////////////////
	void wstrToAnsi( const Menge::StringW& _wstr, Menge::String* _ansi )
	{
		assert( _ansi != NULL );
		const wchar_t* cwstr = _wstr.c_str();
		int size = ::WideCharToMultiByte( CP_ACP, 0, cwstr, -1, 0, 0, 0, 0 );
		std::vector<char> buffer(size);
		::WideCharToMultiByte( CP_ACP, 0, cwstr, -1, &(buffer[0]), size, NULL, NULL );
		_ansi->assign( &(buffer[0]) );
	}
	//////////////////////////////////////////////////////////////////////////
	void utf8ToAnsi( const Menge::String& _utf8, Menge::String* _ansi )
	{
		Menge::StringW wstr;
		utf8ToWstr( _utf8, &wstr );
		wstrToAnsi( wstr, _ansi );
	}
	//////////////////////////////////////////////////////////////////////////
	void ansiToUtf8( const Menge::String& _ansi, Menge::String* _utf8 )
	{
		Menge::StringW wstr;
		ansiToWstr( _ansi, &wstr );
		wstrToUtf8( wstr, _utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool setCurrentDirectory( const Menge::String& _path )
	{
		BOOL result = FALSE;
		if( supportUnicode() == false )
		{
			Menge::String pathAnsi;
			utf8ToAnsi( _path, &pathAnsi );
			result = ::SetCurrentDirectoryA( pathAnsi.c_str() );
		}
		else
		{
			Menge::StringW pathWstr;
			utf8ToWstr( _path, &pathWstr );
			result = ::SetCurrentDirectoryW( pathWstr.c_str() );
		}
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool setModuleCurrentDirectory()
	{
		BOOL result = FALSE;
		if( supportUnicode() == false )
		{
			char exeFilePath[MAX_PATH];
			::GetModuleFileNameA( NULL, exeFilePath, MAX_PATH );
			std::string exeFileDir( exeFilePath );
			std::string::size_type slashPos = exeFileDir.find_last_of( '\\' );
			exeFileDir = exeFileDir.substr( 0, slashPos );
			if( slashPos != std::string::npos )
			{
				result = ::SetCurrentDirectoryA( exeFileDir.c_str() );
			}
		}
		else
		{
			wchar_t exeFilePath[MAX_PATH];
			::GetModuleFileNameW( NULL, exeFilePath, MAX_PATH );
			std::wstring exeFileDir( exeFilePath );
			std::wstring::size_type slashPos = exeFileDir.find_last_of( '\\' );
			exeFileDir = exeFileDir.substr( 0, slashPos );
			if( slashPos != std::wstring::npos )
			{
				result = ::SetCurrentDirectoryW( exeFileDir.c_str() );
			}
		}
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool createDirectory( const Menge::String& _path )
	{
		BOOL result = FALSE;
		if( supportUnicode() == false )
		{
			Menge::String pathAnsi;
			utf8ToAnsi( _path, &pathAnsi );
			result = ::CreateDirectoryA( pathAnsi.c_str(), NULL );
		}
		else
		{
			Menge::StringW pathWstr;
			utf8ToWstr( _path, &pathWstr );
			result = ::CreateDirectoryW( pathWstr.c_str(), NULL );
		}
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	bool fileExists( const Menge::String& _path )
	{
		DWORD attributes = INVALID_FILE_ATTRIBUTES;
		if( supportUnicode() == false )
		{
			Menge::String pathAnsi;
			utf8ToAnsi( _path, &pathAnsi );
			attributes = GetFileAttributesA( pathAnsi.c_str() );
		}
		else
		{
			Menge::StringW pathWstr;
			utf8ToWstr( _path, &pathWstr );
			attributes = GetFileAttributesW( pathWstr.c_str() );
		}
		if( attributes == INVALID_FILE_ATTRIBUTES )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HANDLE createFile( const Menge::String& _filename, DWORD _desiredAccess,
		DWORD _sharedMode, DWORD _creationDisposition )
	{
		if( supportUnicode() == false )
		{
			Menge::String filenameAnsi;
			utf8ToAnsi( _filename, &filenameAnsi );
			return ::CreateFileA( filenameAnsi.c_str(), _desiredAccess, _sharedMode, NULL,
				_creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );
		}
		else
		{
			Menge::StringW filenameWstr;
			utf8ToWstr( _filename, &filenameWstr );
			return ::CreateFileW( filenameWstr.c_str(), _desiredAccess, _sharedMode, NULL,
				_creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
		, HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
		, const char* _menuName, const Menge::String& _className )
	{
		if( supportUnicode() == false )
		{
			Menge::String classNameAnsi;
			utf8ToAnsi( _className, &classNameAnsi );
			WNDCLASSA wc;
			::ZeroMemory( &wc, sizeof(WNDCLASSA) );
			wc.cbClsExtra = _clsExtra;
			wc.cbWndExtra = _wndExtra;
			wc.style = 0;
			wc.lpfnWndProc = _wndProc;
			wc.hInstance = _hInstance;
			wc.hIcon = LoadIconA( _hInstance, MAKEINTRESOURCEA(_hIcon) );
			wc.hCursor = LoadCursorA( NULL, MAKEINTRESOURCEA(32512) );
			wc.lpszClassName = classNameAnsi.c_str();
			wc.hbrBackground = _hbrBackground;
			if( _menuName != NULL )
			{
				Menge::String menuNameAnsi;
				utf8ToAnsi( Menge::String( _menuName ), &menuNameAnsi );
				wc.lpszMenuName = menuNameAnsi.c_str();
			}

			return ::RegisterClassA( &wc );
		}
		else
		{
			Menge::StringW classNameWstr;
			utf8ToWstr( _className, &classNameWstr );
			WNDCLASSW wc;
			::ZeroMemory( &wc, sizeof(WNDCLASSW) );
			wc.cbClsExtra = _clsExtra;
			wc.cbWndExtra = _wndExtra;
			wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = _wndProc;
			wc.hInstance = _hInstance;
			wc.hIcon = LoadIconW( _hInstance, MAKEINTRESOURCEW(_hIcon) );
			wc.hCursor = LoadCursorW( NULL, MAKEINTRESOURCEW(32512) );
			wc.lpszClassName = classNameWstr.c_str();
			wc.hbrBackground = _hbrBackground;

			return ::RegisterClassW( &wc );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL unregisterClass( const Menge::String& _className, HINSTANCE _hInstance )
	{
		if( supportUnicode() == false )
		{
			Menge::String classNameAnsi;
			utf8ToAnsi( _className, &classNameAnsi );
			return ::UnregisterClassA( classNameAnsi.c_str(), _hInstance );
		}
		else
		{
			Menge::StringW classNameWstr;
			utf8ToWstr( _className, &classNameWstr );
			return ::UnregisterClassW( classNameWstr.c_str(), _hInstance );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HWND createWindow( const Menge::String& _className, const Menge::String& _windowName
		, DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance, LPVOID _param )
	{
		if( supportUnicode() == false )
		{
			Menge::String classNameAnsi;
			Menge::String windowNameAnsi;
			utf8ToAnsi( _className, &classNameAnsi );
			utf8ToAnsi( _windowName, &windowNameAnsi );
			return ::CreateWindowA( classNameAnsi.c_str(), windowNameAnsi.c_str()
				, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );
		}
		else
		{
			Menge::StringW classNameWstr;
			Menge::StringW windowNameWstr;
			utf8ToWstr( _className, &classNameWstr );
			utf8ToWstr( _windowName, &windowNameWstr );
			return ::CreateWindowW( classNameWstr.c_str(), windowNameWstr.c_str()
				, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HWND createWindowEx( DWORD _exStyle, const Menge::String& _className
		, const Menge::String& _windowName,	DWORD _style, int _x, int _y
		, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance,	LPVOID _param )
	{
		if( supportUnicode() == false )
		{
			Menge::String classNameAnsi;
			Menge::String windowNameAnsi;
			utf8ToAnsi( _className, &classNameAnsi );
			utf8ToAnsi( _windowName, &windowNameAnsi );
			return ::CreateWindowExA( _exStyle, classNameAnsi.c_str(), windowNameAnsi.c_str()
				, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );
		}
		else
		{
			Menge::StringW classNameWstr;
			Menge::StringW windowNameWstr;
			utf8ToWstr( _className, &classNameWstr );
			utf8ToWstr( _windowName, &windowNameWstr );
			return ::CreateWindowExW( _exStyle, classNameWstr.c_str(), windowNameWstr.c_str()
				, _style, _x, _y, _width, _height, _parent, _hMenu, _hInstance, _param );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LPVOID getCreateParams( LPARAM _lParam )
	{
		if( supportUnicode() == false )
		{
			LPCREATESTRUCTA createStruct = (LPCREATESTRUCTA)_lParam;
			return createStruct->lpCreateParams;
		}
		else
		{
			LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)_lParam;
			return createStruct->lpCreateParams;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong )
	{
		return supportUnicode() ?
			::SetWindowLongW( _hWnd, _index, _newLong ) :
			::SetWindowLongA( _hWnd, _index, _newLong );
	}
	//////////////////////////////////////////////////////////////////////////
	LONG getWindowLong( HWND _hWnd,	int _index )
	{
		return supportUnicode() ?
			::GetWindowLongW( _hWnd, _index ) :
			::GetWindowLongA( _hWnd, _index );
	}
	//////////////////////////////////////////////////////////////////////////
	LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong )
	{
		return supportUnicode() ? 
			::SetWindowLongPtrW( _hWnd, _index, _newLong ) :
			::SetWindowLongPtrA( _hWnd, _index, _newLong );
	}
	//////////////////////////////////////////////////////////////////////////
	LONG_PTR getWindowLongPtr( HWND _hWnd, int _index )
	{
		return supportUnicode() ?
			::GetWindowLongPtrW( _hWnd, _index ) :
			::GetWindowLongPtrA( _hWnd, _index );
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam )
	{
		return supportUnicode() ?
			::DefWindowProcW( _hWnd, _msg, _wParam, _lParam ) :
			::DefWindowProcA( _hWnd, _msg, _wParam, _lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	void getCurrentDirectory( Menge::String* _path )
	{
		assert( _path != NULL );

		if( supportUnicode() == false )
		{
			char buffer[MAX_PATH];
			::GetCurrentDirectoryA( MAX_PATH, buffer );
			ansiToUtf8( Menge::String( buffer ), _path );
		}
		else
		{
			wchar_t buffer[MAX_PATH];
			::GetCurrentDirectoryW( MAX_PATH, buffer );
			wstrToUtf8( Menge::StringW( buffer ), _path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax
		, UINT _removeMsg )
	{
		return supportUnicode() ?
			::PeekMessageW( _msg, _hWnd, _msgFilterMin, _msgFilterMax, _removeMsg ) :
			::PeekMessageA(  _msg, _hWnd, _msgFilterMin, _msgFilterMax, _removeMsg );
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT dispatchMessage( const MSG* _msg )
	{
		return supportUnicode() ?
			::DispatchMessageW( _msg ) :
			::DispatchMessageA( _msg );
	}
	//////////////////////////////////////////////////////////////////////////
	int messageBox( HWND _hWnd, const Menge::String& _text,	const Menge::String& _caption
		, UINT _type )
	{
		int result = 0;
		if( supportUnicode() == false )
		{
			Menge::String textAnsi;
			Menge::String captionAnsi;
			utf8ToAnsi( _text, &textAnsi );
			utf8ToAnsi( _caption, &captionAnsi );
			result = ::MessageBoxA( _hWnd, textAnsi.c_str(), captionAnsi.c_str(), _type );
		}
		else
		{
			Menge::StringW textWstr;
			Menge::StringW captionWstr;
			utf8ToWstr( _text, &textWstr );
			utf8ToWstr( _caption, &captionWstr );
			result = ::MessageBoxW( _hWnd, textWstr.c_str(), captionWstr.c_str(), _type );
		}
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	HDESK openDesktop( const Menge::String& lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess )
	{
		HDESK result = NULL;
		if( supportUnicode() == false)
		{
			Menge::String desctopAnsi;
			utf8ToAnsi(lpszDesktop, &desctopAnsi);
			result = ::OpenDesktopA(desctopAnsi.c_str(), dwFlags, fInherit, dwDesiredAccess);
		}
		else
		{
			Menge::StringW desctopWstr;
			utf8ToWstr(lpszDesktop, &desctopWstr);
			result = ::OpenDesktopW(desctopWstr.c_str(), dwFlags, fInherit, dwDesiredAccess);
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void getModuleFileName( HMODULE hModule, Menge::String* _moduleFilename )
	{
		if( supportUnicode() == false )
		{
			char exeFilePath[MAX_PATH];
			::GetModuleFileNameA( hModule, exeFilePath, MAX_PATH );
			ansiToUtf8(exeFilePath, _moduleFilename);
		}
		else
		{
			wchar_t exeFilePath[MAX_PATH];
			::GetModuleFileNameW( hModule, exeFilePath, MAX_PATH );
			wstrToUtf8(exeFilePath, _moduleFilename);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LONG setRegistryValue( HKEY _hKey, Menge::String _lpKeyName, Menge::String _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData )
	{
		HKEY openedKey; 
		LONG result;

		bool isStringValue = _dwType == REG_SZ || _dwType == REG_MULTI_SZ || _dwType == REG_EXPAND_SZ;
		
		/*Menge::String keyName;
		size_t separatorIndex = _lpValueName.find_last_of("\\");
		keyName = _lpValueName.substr(0, separatorIndex);
		_lpValueName = _lpValueName.substr(separatorIndex + 1);*/

		if( supportUnicode() == true )
		{
			Menge::StringW valueNameW;
			Menge::StringW keyNameW;
			Menge::StringW lpDataW;

			utf8ToWstr(_lpValueName, &valueNameW);
			utf8ToWstr(_lpKeyName, &keyNameW);
			
			::RegOpenKeyExW(_hKey, keyNameW.c_str(), NULL, KEY_ALL_ACCESS, &openedKey);

			if( isStringValue == true )
			{
				Menge::StringA str( reinterpret_cast<const char*>( _lpData ) );
				utf8ToWstr(str, &lpDataW);
				_cbData = static_cast<DWORD>( (lpDataW.length()+1) * 2 );
				_lpData = reinterpret_cast<const BYTE*>( lpDataW.c_str() );
			}

			result = ::RegSetValueExW(openedKey, valueNameW.c_str(), NULL, _dwType, _lpData, _cbData );
		}
		else
		{
			Menge::StringA valueNameA;
			Menge::StringA keyNameA;
			Menge::StringA lpDataA;

			utf8ToAnsi(_lpValueName, &valueNameA);
			utf8ToAnsi(_lpKeyName, &keyNameA);

			::RegOpenKeyExA(_hKey, keyNameA.c_str(), NULL, KEY_ALL_ACCESS, &openedKey);

			if( isStringValue == true )
			{
				Menge::StringA str( reinterpret_cast<const char*>( _lpData ) );
				utf8ToAnsi(str, &lpDataA);
				_cbData = static_cast<DWORD>( (lpDataA.length()+1) );
				_lpData = reinterpret_cast<const BYTE*>( lpDataA.c_str() );
			}

			result = ::RegSetValueExA(openedKey, valueNameA.c_str(), NULL, _dwType, _lpData, _cbData );
			
		}
		
		::RegCloseKey( openedKey );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	LONG deleteRegistryValue( HKEY _hKey, Menge::String _lpKeyName, Menge::String _lpValueName )
	{
		HKEY openedKey;
		LONG result;

		if( supportUnicode() == true )
		{
			Menge::StringW valueNameW;
			Menge::StringW keyNameW;

			utf8ToWstr(_lpValueName, &valueNameW);
			utf8ToWstr(_lpKeyName, &keyNameW);

			::RegOpenKeyExW(_hKey, keyNameW.c_str(), NULL, KEY_ALL_ACCESS, &openedKey);

			result = ::RegDeleteValueW(openedKey, valueNameW.c_str() );
		}
		else
		{
			Menge::StringA valueNameA;
			Menge::StringA keyNameA;

			utf8ToAnsi(_lpValueName, &valueNameA);
			utf8ToAnsi(_lpKeyName, &keyNameA);

			::RegOpenKeyExA(_hKey, keyNameA.c_str(), NULL, KEY_ALL_ACCESS, &openedKey);

			result = ::RegDeleteValueA(openedKey, valueNameA.c_str() );
		}
		
		::RegCloseKey( openedKey );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void getShortPathName( const Menge::String& _longPathName, Menge::String* _shortPathName )
	{
		assert( _shortPathName != NULL );
		_shortPathName->clear();

		if( supportUnicode() == true )
		{
			Menge::StringW longPathNameW;
			utf8ToWstr( _longPathName, &longPathNameW );
			wchar_t shortPathNameW[MAX_PATH];
			if( ::GetShortPathNameW( longPathNameW.c_str(), shortPathNameW, MAX_PATH ) != 0 )
			{
				wstrToUtf8( shortPathNameW, _shortPathName );
			}
		}
		else
		{
			Menge::StringA longPathNameA;
			utf8ToAnsi( _longPathName, &longPathNameA );
			char shortPathNameA[MAX_PATH];
			if( ::GetShortPathNameA( longPathNameA.c_str(), shortPathNameA, MAX_PATH ) != 0 )
			{
				ansiToUtf8( shortPathNameA, _shortPathName );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace WindowsLayer
