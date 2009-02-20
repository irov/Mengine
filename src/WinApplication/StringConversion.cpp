#	include "StringConversion.h"

namespace StringConversion
{
	//////////////////////////////////////////////////////////////////////////
	Menge::StringW s_UTF8ToWChar( const Menge::String& _utf8 )
	{
		int size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, 0, 0 );
		wchar_t* conv = new wchar_t[size];
		MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, conv, size );
		Menge::StringW out( conv );
		delete[] conv;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String s_WCharToUTF8( const WCHAR* _wchar )
	{
		int size = WideCharToMultiByte( CP_UTF8, 0, _wchar, -1, 0, 0, 0, 0 );
		char* conv = new char[size];
		WideCharToMultiByte( CP_UTF8, 0, _wchar, -1, conv, size, NULL, NULL );
		Menge::String out( conv );
		delete[] conv;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String s_ansiToUtf8( const Menge::String& _ansi )
	{
		int wide_size = MultiByteToWideChar( CP_ACP, 0, _ansi.c_str(), -1, NULL, 0 );
		wchar_t* wide = new wchar_t[wide_size];
		MultiByteToWideChar( CP_ACP, 0, _ansi.c_str(), -1, wide, wide_size );
		int utf8_size = WideCharToMultiByte( CP_UTF8, 0, wide, wide_size, NULL, 0, NULL, NULL );
		char* utf8 = new char[utf8_size];
		WideCharToMultiByte( CP_UTF8, 0, wide, wide_size, utf8, utf8_size, NULL, NULL );
		Menge::String out( utf8 );
		delete[] wide;
		delete[] utf8;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String s_utf8ToAnsi( const Menge::String& _utf8 )
	{
		int wide_size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, NULL, 0 );
		wchar_t* wide = new wchar_t[wide_size];
		MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, wide, wide_size );
		int anis_size = WideCharToMultiByte( CP_ACP, 0, wide, wide_size, NULL, 0, NULL, NULL );
		char* ansi = new char[anis_size];
		WideCharToMultiByte( CP_ACP, 0, wide, wide_size, ansi, anis_size, NULL, NULL );
		Menge::String out( ansi );
		delete[] wide;
		delete[] ansi;
		return out;
	}
}