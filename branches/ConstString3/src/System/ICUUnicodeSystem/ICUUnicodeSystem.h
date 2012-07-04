#	pragma once

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class ICUUnicodeService
		: public UnicodeServiceInterface
	{
	public:
		ICUUnicodeService();

	public:
		void initialize( LogServiceInterface * _logService ) override;

	public:
		String unicodeToUtf8( const WString& _unicode, bool & _successful ) override;
		WString utf8ToUnicode( const String& _utf8, bool & _successful ) override;

	protected:
		LogServiceInterface * m_logService;
	};
}
