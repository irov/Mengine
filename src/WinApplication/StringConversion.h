#	pragma once

#	include "Menge/Utils.h"
#	include <windows.h>

namespace StringConversion	
{
	Menge::StringW s_UTF8ToWChar( const Menge::String& _utf8 );
	Menge::String s_WCharToUTF8( const WCHAR* _wchar );
	Menge::String s_ansiToUtf8( const Menge::String& _ansi );
	Menge::String s_utf8ToAnsi( const Menge::String& _utf8 );
}
