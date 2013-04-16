#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class LogServiceInterface;

    class UnicodeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("UnicodeSystem");

    public:
        virtual bool unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utf8Size ) = 0;
        virtual bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;

        virtual bool utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize ) = 0;
        virtual bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
    };


#   define UNICODE_SYSTEM( serviceProvider )\
    (Menge::getService<Menge::UnicodeSystemInterface>(serviceProvider))

	class UnicodeServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("UnicodeService");

	public:
        virtual bool unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utf8Size ) = 0;
        virtual bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;

        virtual bool utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize ) = 0;
        virtual bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
	};

#   define UNICODE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::UnicodeServiceInterface>(serviceProvider))

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8Size( ServiceProviderInterface * _serviceProvide, const WString::value_type * _unicode, WString::size_type _unicodeSize , String & _utf8 )
        {
            UnicodeServiceInterface * unicodeService = UNICODE_SERVICE(_serviceProvide);

            if( unicodeService == NULL )
            {
                return false;
            }

            size_t utf8Size;
            if( unicodeService->unicodeToUtf8Size( _unicode, _unicodeSize, &utf8Size ) == false )
            {
                return false;
            }

            if( utf8Size == 0 )
            {
                _utf8.clear();

                return true;
            }

            _utf8.resize( utf8Size );

            if( unicodeService->unicodeToUtf8( _unicode, _unicodeSize, &_utf8[0], utf8Size, 0 ) == false )
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

            if( unicodeService == NULL )
            {
                return false;
            }

            size_t unicodeSize;
            if( unicodeService->utf8ToUnicodeSize( _utf8, _utf8Size, &unicodeSize ) == false )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode.clear();

                return true;
            }

            _unicode.resize( unicodeSize );

            if( unicodeService->utf8ToUnicode( _utf8, _utf8Size, &_unicode[0], unicodeSize, 0 ) == false )
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
    }
}
