#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class LogServiceInterface;

	class UnicodeInterface
		: public ServiceInterface
	{
	public:
		virtual void initialize( LogServiceInterface * _logService ) = 0;

	public:
		virtual String unicodeToUtf8( const WString& _unicode, bool & _successful ) = 0;
		virtual WString utf8ToUnicode( const String& _utf8, bool & _successful ) = 0;
	};
}

bool initInterfaceSystem( Menge::UnicodeInterface **_system );
void releaseInterfaceSystem( Menge::UnicodeInterface *_system );