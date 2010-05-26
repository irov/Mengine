/*
 *	WindowsLayer.h
 *
 *	Created by _Berserk_ on 10.3.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#pragma once

// support Windows 98 and later
#ifndef WINVER
#define WINVER 0x0410
#endif

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // WIN98
#endif

#include <Windows.h>

#include "Config/Typedef.h"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA                     120
#endif

namespace WindowsLayer
{
	enum EWindowsType
	{
		EWT_UNKNOWN,
		EWT_NT,
		EWT_98,
		EWT_VISTA
	};
	
	EWindowsType getWindowsType();
	void setProcessDPIAware();
	bool supportUnicode();
	void utf8ToWstr( const Menge::String& _utf8, Menge::StringW* _wstr );
	void wstrToUtf8( const Menge::StringW& _wstr, Menge::String* _utf8 );
	void ansiToWstr( const Menge::String& _ansi, Menge::StringW* _wstr );
	void wstrToAnsi( const Menge::StringW& _wstr, Menge::String* _ansi );
	void utf8ToAnsi( const Menge::String& _utf8, Menge::String* _ansi );
	void ansiToUtf8( const Menge::String& _ansi, Menge::String* _utf8 );
	bool setCurrentDirectory( const Menge::String& _path );
	bool setModuleCurrentDirectory();
	bool createDirectory( const Menge::String& _path );
	bool fileExists( const Menge::String& _path );
	HANDLE createFile( const Menge::String& _filename, DWORD _desiredAccess,
		DWORD _sharedMode, DWORD _creationDisposition );
	ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
		, HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
		, const char* _menuName, const Menge::String& _className );
	BOOL unregisterClass( const Menge::String& _className, HINSTANCE _hInstance );
	HWND createWindow( const Menge::String& _className, const Menge::String& _windowName
		, DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance, LPVOID _param );
	HWND createWindowEx( DWORD _exStyle, const Menge::String& _className
		, const Menge::String& _windowName,	DWORD _style, int _x, int _y
		, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance,	LPVOID _param );
	LPVOID getCreateParams( LPARAM _lParam );
	LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong );
	LONG getWindowLong( HWND _hWnd,	int _index );
	LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong );
	LONG_PTR getWindowLongPtr( HWND _hWnd, int _index );
	LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam );
	void getCurrentDirectory( Menge::String* _path );
	BOOL peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax
		, UINT _removeMsg );
	LRESULT dispatchMessage( const MSG* _msg );
	int messageBox( HWND _hWnd, const Menge::String& _text,	const Menge::String& _caption
		, UINT _type );
	HDESK openDesktop(const Menge::String& lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess);
	void getModuleFileName( HMODULE hModule, Menge::String* _moduleFilename );
	LONG setRegistryValue( HKEY _hKey, Menge::String _lpKeyName, Menge::String _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
	LONG deleteRegistryValue( HKEY _hKey, Menge::String _lpKeyName, Menge::String _lpValueName );
	void getShortPathName( const Menge::String& _longPathName, Menge::String* _shortPathName );
}	// namespace WindowsLayer
