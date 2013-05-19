#	pragma once

#	include "Interface/UnicodeInterface.h"

namespace Menge
{
	class Win32UnicodeSystem
		: public UnicodeSystemInterface
	{
	public:
		Win32UnicodeSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        void destroy() override;

	public:
        bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;
        bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}
