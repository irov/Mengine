#	pragma once

#	include "Menge/Utils.h"
#	include <windows.h>

namespace StringConversion	
{
	Menge::StringW utf8ToWChar( const Menge::String& _utf8 );
	Menge::String wcharToUTF8( const WCHAR* _wchar );
	Menge::String ansiToUtf8( const Menge::String& _ansi );
	Menge::String utf8ToAnsi( const Menge::String& _utf8 );
}
