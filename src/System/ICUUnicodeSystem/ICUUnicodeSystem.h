#	pragma once

#	include "Interface/UnicodeInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class ICUUnicodeSystem
		: public UnicodeSystemInterface
	{
	public:
		ICUUnicodeSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
        bool unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utf8Size ) override;
        bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;

        bool utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize ) override;
        bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}
