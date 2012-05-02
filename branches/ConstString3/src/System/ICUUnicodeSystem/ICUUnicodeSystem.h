#	pragma once

#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	class ICUUnicodeService
		: public UnicodeInterface
	{
	public:
		String unicodeToUtf8( const WString& _unicode, bool & _successful ) override;
		WString utf8ToUnicode( const String& _utf8, bool & _successful ) override;
	};
}
