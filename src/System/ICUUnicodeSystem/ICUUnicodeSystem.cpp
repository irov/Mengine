#	include "ICUUnicodeSystem.h"

#	include "unicode/ustring.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::UnicodeSystemInterface, Menge::ICUUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ICUUnicodeSystem::ICUUnicodeSystem()
		: m_serviceProvider(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ICUUnicodeSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ICUUnicodeSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeSystem::unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utfSize )
    {
        size_t str_buffer_size = _unicodeSize * 2;
        UChar * str_buffer = new UChar[str_buffer_size];
                 
        int32_t str_len = 0; 

        UErrorCode str_status = U_ZERO_ERROR;
        u_strFromWCS( 
            str_buffer,
            str_buffer_size, 
            &str_len, 
            _unicode, 
            _unicodeSize, 
            &str_status 
            );

        if( U_FAILURE(str_status) ) 
        { 
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::unicodeToUtf8 size %ls - err %s"
                , _unicode
                , u_errorName(str_status)
                );

            delete [] str_buffer;

            return false;
        }

        int32_t utf8_len;

        UErrorCode utf8_status = U_ZERO_ERROR;
        u_strToUTF8(NULL, 0, &utf8_len, str_buffer, str_len, &utf8_status );

        delete [] str_buffer;

        if( U_FAILURE(utf8_status) && (utf8_status != U_BUFFER_OVERFLOW_ERROR)) 
        {			
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::unicodeToUtf8 size %ls - err %s"
                , _unicode
                , u_errorName(utf8_status)
                );

            return false;
        }

        *_utfSize = utf8_len;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeSystem::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        size_t str_buffer_size = _unicodeSize;
        UChar * str_buffer = new UChar[str_buffer_size];

        int32_t str_len = 0; 

        UErrorCode str_status = U_ZERO_ERROR;
        u_strFromWCS( 
            str_buffer,
            str_buffer_size, 
            &str_len, 
            _unicode, 
            _unicodeSize, 
            &str_status 
            );

        if( U_FAILURE(str_status) ) 
        { 
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::unicodeToUtf8 size %ls - err %s"
                , _unicode
                , u_errorName(str_status)
                );

            delete [] str_buffer;

            return false;
        }

        UErrorCode utf8_status = U_ZERO_ERROR;

        int32_t utf8_len;
        u_strToUTF8( _utf8, _utf8Capacity, &utf8_len, str_buffer, str_len, &utf8_status );

        delete [] str_buffer;

        if( U_FAILURE(utf8_status) ) 
        {
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::unicodeToUtf8 conv %ls - err %s"
                , _unicode
                , u_errorName(utf8_status)
                );

            return false;
        }

        if( _utf8Size != NULL )
        {
            *_utf8Size = utf8_len;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeSystem::utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize )
    {
        size_t str_buffer_size = _utf8Size;
        UChar * str_buffer = new UChar[str_buffer_size];

        int32_t str_len;

        UErrorCode str_status = U_ZERO_ERROR;
        u_strFromUTF8(
            str_buffer, 
            str_buffer_size, 
            &str_len, 
            _utf8, 
            _utf8Size, 
            &str_status 
            );
        
        if( U_FAILURE(str_status) && (str_status != U_BUFFER_OVERFLOW_ERROR)) 
        {
            const char * status = u_errorName(str_status);
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::utf8ToUnicode size |%s| - err '%s'"
                , _utf8
                , status
                );

            delete [] str_buffer;

            return false;
        }

        int32_t wcs_len;

        UErrorCode wcs_status = U_ZERO_ERROR;
        u_strToWCS( 
            NULL, 
            0, 
            &wcs_len, 
            str_buffer, 
            str_len, 
            &wcs_status 
            ); 

        delete [] str_buffer;

        if( U_FAILURE(wcs_status) && (wcs_status != U_BUFFER_OVERFLOW_ERROR) ) 
        {
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::utf8ToUnicode u_strToWCS %s - err %s"
                , _utf8
                , u_errorName(wcs_status)
                );

            return false;
        }

        *_unicodeSize = wcs_len;
        
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ICUUnicodeSystem::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{
        size_t str_buffer_size = _utf8Size;
        UChar * str_buffer = new UChar[str_buffer_size];

		int32_t str_len;

        UErrorCode str_status = U_ZERO_ERROR;
		u_strFromUTF8(
            str_buffer, 
            str_buffer_size, 
            &str_len, 
            _utf8, 
            _utf8Size, 
            &str_status 
            );

		if( U_FAILURE(str_status) ) 
		{
			LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::utf8ToUnicode conv %s - err %s"
				, _utf8
				, u_errorName(str_status)
				);

            delete [] str_buffer;
			
			return false;
		}

        int32_t wcs_len;

        UErrorCode wcs_status = U_ZERO_ERROR;
        u_strToWCS( 
            _unicode, 
            _unicodeCapacity, 
            &wcs_len, 
            str_buffer, 
            str_len, 
            &wcs_status 
            ); 

        delete [] str_buffer;

        if( U_FAILURE(wcs_status) ) 
        {
            LOGGER_ERROR(m_serviceProvider)("ICUUnicodeService::utf8ToUnicode u_strToWCS %s - err %s"
                , _utf8
                , u_errorName(wcs_status)
                );

            return false;
        }

        if( _sizeUnicode != NULL )
        {
		    *_sizeUnicode = wcs_len;
        }

		return true;
	}
}