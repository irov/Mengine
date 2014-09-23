#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"

namespace Menge
{
    const size_t UNICODE_UNSIZE = (size_t)-1;

    class UnicodeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("UnicodeSystem");

    public:
        virtual bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;
        virtual bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
    };


#   define UNICODE_SYSTEM( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::UnicodeSystemInterface)

	class UnicodeServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("UnicodeService");

	public:
        virtual bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;
        virtual bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
	};

#   define UNICODE_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::UnicodeServiceInterface)

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8Size( ServiceProviderInterface * _serviceProvide, const WString::value_type * _unicode, WString::size_type _unicodeSize, String & _utf8 )
        {
            UnicodeServiceInterface * unicodeService = UNICODE_SERVICE(_serviceProvide);

            size_t utf8Size;
            if( unicodeService->unicodeToUtf8( _unicode, _unicodeSize, nullptr, 0, &utf8Size ) == false )
            {
                return false;
            }

            if( utf8Size == 0 )
            {
                _utf8.clear();

                return true;
            }

            _utf8.resize( utf8Size );

            if( unicodeService->unicodeToUtf8( _unicode, _unicodeSize, &_utf8[0], utf8Size, nullptr ) == false )
            {
                _utf8.clear();

                return false;                 
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8( ServiceProviderInterface * _serviceProvide, const WString & _unicode, String & _utf8 )
        {
            bool result = unicodeToUtf8Size( _serviceProvide, _unicode.c_str(), _unicode.size(), _utf8 );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool utf8ToUnicodeSize( ServiceProviderInterface * _serviceProvide, const String::value_type * _utf8, String::size_type _utf8Size, WString & _unicode )
        {
            UnicodeServiceInterface * unicodeService = UNICODE_SERVICE( _serviceProvide );

            size_t unicodeSize;
            if( unicodeService->utf8ToUnicode( _utf8, _utf8Size, nullptr, 0, &unicodeSize ) == false )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode.clear();

                return true;
            }

            _unicode.resize( unicodeSize );

            if( unicodeService->utf8ToUnicode( _utf8, _utf8Size, &_unicode[0], unicodeSize, nullptr ) == false )
            {
                _unicode.clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool utf8ToUnicode( ServiceProviderInterface * _serviceProvide, const String & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _serviceProvide, _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
		//////////////////////////////////////////////////////////////////////////
		inline bool utf8ToUnicode( ServiceProviderInterface * _serviceProvide, const ConstString & _utf8, WString & _unicode )
		{
			bool result = utf8ToUnicodeSize( _serviceProvide, _utf8.c_str(), _utf8.size(), _unicode );

			return result;
		}
    }
}
