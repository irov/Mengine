#	pragma once

#	include "Interface/ApplicationInterface.h"
#	include "windows.h"

struct CharConvUtils
{
	static Menge::StringW UTF8ToWChar( const Menge::String& _utf8 );
	static Menge::String WCharToAnsi( const Menge::StringW& _widechar );
};