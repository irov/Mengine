#	include "CharConvUtils.h"

//////////////////////////////////////////////////////////////////////////
Menge::StringW CharConvUtils::UTF8ToWChar( const Menge::String& _utf8 )
{
	int size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, 0, 0 );
	wchar_t* conv = new wchar_t[size];
	MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, conv, size );
	Menge::StringW out( conv );
	delete[] conv;
	return out;
}
//////////////////////////////////////////////////////////////////////////
Menge::String CharConvUtils::WCharToAnsi( const Menge::StringW& _widechar )
{
	int size = WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, NULL, 0, NULL, NULL );
	char* conv = new char[size];
	WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, conv, size, NULL, NULL );
	Menge::String out( conv );
	delete[] conv;
	return out;
}
//////////////////////////////////////////////////////////////////////////