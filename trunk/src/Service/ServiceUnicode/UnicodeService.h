#	pragma once

#   include "Interface/UnicodeInterface.h"

namespace Menge
{
	class UnicodeService
        : public UnicodeServiceInterface
	{	
    public:
        UnicodeService();
        ~UnicodeService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) override;
        bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}
