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
        bool unicodeToUtf8Size( const WChar * _unicode, size_t _unicodeSize, size_t * _utf8Size ) override;
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;

        bool utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize ) override;
        bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode ) override;

	protected:
		LogServiceInterface * m_logService;
	};
}
