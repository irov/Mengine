#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
    const size_t UNICODE_UNSIZE = (size_t)-1;

    class UnicodeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("UnicodeSystem");

    public:
        virtual bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;
        virtual bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
    };


#   define UNICODE_SYSTEM()\
    SERVICE_GET(Menge::UnicodeSystemInterface)

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8Size( const WString::value_type * _unicode, WString::size_type _unicodeSize, String & _utf8 )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

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
        inline bool unicodeToUtf8( const WString & _unicode, String & _utf8 )
        {
            bool result = unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), _utf8 );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8( const WChar * _unicode, Char * _utf8, size_t _utf8Capacity )
        {
            size_t unicode_size = wcslen( _unicode );

            size_t utf8_size;
            if( UNICODE_SYSTEM()
                ->unicodeToUtf8( _unicode, unicode_size, _utf8, _utf8Capacity, &utf8_size ) == false )
            {
                return false;
            }

            _utf8[utf8_size] = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool unicodeToUtf8( const WString & _unicode, Char * _utf8, size_t _utf8Capacity )
        {
            const WChar * unicode_str = _unicode.c_str();
            size_t unicode_size = _unicode.size();

            size_t utf8_size;
            if( UNICODE_SYSTEM()
                ->unicodeToUtf8( unicode_str, unicode_size, _utf8, _utf8Capacity, &utf8_size ) == false )
            {
                return false;
            }

            _utf8[utf8_size] = '\0';

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool utf8ToUnicodeSize( const String::value_type * _utf8, String::size_type _utf8Size, WString & _unicode )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

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
        inline bool utf8ToUnicode( const String & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
		//////////////////////////////////////////////////////////////////////////
		inline bool utf8ToUnicode( const ConstString & _utf8, WString & _unicode )
		{
			bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

			return result;
		}
        //////////////////////////////////////////////////////////////////////////
        inline bool utf8ToUnicode( const PathString & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool utf8ToUnicode( const FilePath & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
    }
}
