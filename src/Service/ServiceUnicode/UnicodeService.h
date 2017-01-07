#	pragma once

#   include "Interface/UnicodeInterface.h"

#   include "Core/ServiceBase.h"

namespace Menge
{
	class UnicodeService
        : public ServiceBase<UnicodeServiceInterface>
	{	
    public:
        UnicodeService();
        ~UnicodeService();

    public:
        bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;
        bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) override;
	};
}
