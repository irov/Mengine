#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class LogServiceInterface;

	class UnicodeServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void initialize( LogServiceInterface * _logService ) = 0;

	public:
        virtual bool unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utf8Size ) = 0;
		virtual bool unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;

        virtual bool utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize ) = 0;
		virtual bool utf8ToUnicode( const char * _utf8, size_t _utf8Size, wchar_t * _unicode, size_t _unicodeCapacity, size_t * _size ) = 0;
	};

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline bool UnicodeToUtf8( UnicodeServiceInterface * _unicodeService, const WString & _unicode, String & _utf8 )
        {
            size_t utf8Size;

            if( _unicodeService->unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), &utf8Size ) == false )
            {
                return false;
            }
                        
            String::value_type * buffer = new String::value_type[utf8Size];
            
            if( _unicodeService->unicodeToUtf8( _unicode.c_str(), _unicode.size(), buffer, utf8Size, 0 ) == false )
            {
                delete [] buffer;
            }

            _utf8.assign( buffer, utf8Size );

            delete [] buffer;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        inline bool Utf8ToUnicode( UnicodeServiceInterface * _unicodeService, const String & _utf8, WString & _unicode )
        {
            size_t unicodeSize;

            if( _unicodeService->utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), &unicodeSize ) == false )
            {
                return false;
            }

            WString::value_type * buffer = new WString::value_type[unicodeSize];

            if( _unicodeService->utf8ToUnicode( _utf8.c_str(), _utf8.size(), buffer, unicodeSize, 0 ) == false )
            {
                delete [] buffer;
            }

            _unicode.assign( buffer, unicodeSize );

            delete [] buffer;

            return true;
        }
    }
}

bool initInterfaceSystem( Menge::UnicodeServiceInterface **_system );
void releaseInterfaceSystem( Menge::UnicodeServiceInterface *_system );