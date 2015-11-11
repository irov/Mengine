#	pragma once

#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	class MarmaladeUnicodeSystem
		: public ServiceBase<UnicodeSystemInterface>
	{
	public:
		MarmaladeUnicodeSystem();

	public:
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode ) override;
	};
}
