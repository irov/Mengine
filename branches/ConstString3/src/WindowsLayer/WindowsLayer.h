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
#define WINVER 0x0500
#endif

//#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
//#define _WIN32_WINDOWS 0x0410 // WIN98
//#endif

#include <Windows.h>
#include <WinUser.h>

#include "Config/Typedef.h"

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

	void utf8ToUnicode( const Menge::String& _utf8, Menge::WString & _wstr );
	void unicodeToUtf8( const Menge::WString& _wstr, Menge::String & _utf8 );
	void ansiToUnicode( const Menge::String& _ansi, Menge::WString & _wstr );
	void unicodeToAnsi( const Menge::WString& _wstr, Menge::String & _ansi );

	void utf8ToAnsi( const Menge::String& _utf8, Menge::String & _ansi );
	void ansiToUtf8( const Menge::String& _ansi, Menge::String & _utf8 );
	void utf8Count( const Menge::String& _utf8, size_t & _size );

	bool setCurrentDirectory( const Menge::WString& _path );
	bool setModuleCurrentDirectory();
	bool createDirectory( const Menge::WString& _path );
	bool fileExists( const Menge::WString& _path );

	HANDLE createFile( const Menge::WString& _filename, DWORD _desiredAccess,
		DWORD _sharedMode, DWORD _creationDisposition );

	ATOM registerClass( WNDPROC _wndProc, int _clsExtra, int _wndExtra
		, HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
		, const Menge::WString& _className );

	BOOL unregisterClass( const Menge::WString& _className, HINSTANCE _hInstance );

	HWND createWindow( const Menge::WString& _className, const Menge::WString& _windowName
		, DWORD _style, int _x, int _y, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance, LPVOID _param );

	HWND createWindowEx( DWORD _exStyle, const Menge::WString& _className
		, const Menge::WString& _windowName,	DWORD _style, int _x, int _y
		, int _width, int _height, HWND _parent, HMENU _hMenu
		, HINSTANCE _hInstance,	LPVOID _param );

	void destroyWindow( HWND _hwnd );
	LPVOID getCreateParams( LPARAM _lParam );
	LONG setWindowLong( HWND _hWnd,	int _index,	LONG _newLong );
	LONG getWindowLong( HWND _hWnd,	int _index );
	LONG_PTR setWindowLongPtr( HWND _hWnd, int _index, LONG_PTR _newLong );
	LONG_PTR getWindowLongPtr( HWND _hWnd, int _index );
	LRESULT defWindowProc( HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam );
	void getCurrentDirectory( Menge::WString & _path );
	BOOL peekMessage( LPMSG _msg, HWND _hWnd, UINT _msgFilterMin, UINT _msgFilterMax
		, UINT _removeMsg );
	LRESULT dispatchMessage( const MSG* _msg );
	
	int messageBox( HWND _hWnd, const Menge::WString& _text, const Menge::WString& _caption, UINT _type );

	void getModuleFileName( HMODULE hModule, Menge::WString & _moduleFilename );
	LONG setRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName, DWORD _dwType, const BYTE* _lpData, DWORD _cbData );
	LONG deleteRegistryValue( HKEY _hKey, const Menge::WString & _lpKeyName, const Menge::WString & _lpValueName );
	bool getShortPathName( const Menge::WString& _longPathName, Menge::WString & _shortPathName );
}	// namespace WindowsLayer
