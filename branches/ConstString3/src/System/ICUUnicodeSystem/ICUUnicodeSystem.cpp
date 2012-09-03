#	include "ICUUnicodeSystem.h"

#	include "unicode/ustring.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::UnicodeServiceInterface **_system )
{
	try
	{
		*_system = new Menge::ICUUnicodeService();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::UnicodeServiceInterface *_system )
{
	delete static_cast<Menge::ICUUnicodeService*>( _system );
}
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ICUUnicodeService::ICUUnicodeService()
		: m_logService(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ICUUnicodeService::initialize( LogServiceInterface * _logService )
	{
		m_logService = _logService;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeService::unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utfSize )
    {
        int32_t utf8_len;

        UErrorCode err_code_len = U_ZERO_ERROR;
        u_strToUTF8(NULL, 0, &utf8_len, _unicode, _unicodeSize, &err_code_len );

        if( U_FAILURE(err_code_len) && (err_code_len != U_BUFFER_OVERFLOW_ERROR)) 
        {			
            LOGGER_ERROR(m_logService)("ICUUnicodeService::unicodeToUtf8 size %S - err %s"
                , _unicode
                , u_errorName(err_code_len)
                );

            return false;
        }

        *_utfSize = utf8_len;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeService::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        UErrorCode err_code_conv = U_ZERO_ERROR;

        int32_t result_len;
        u_strToUTF8( _utf8, _utf8Capacity, &result_len, _unicode, _unicodeSize, &err_code_conv );

        if( U_FAILURE(err_code_conv) ) 
        {
            LOGGER_ERROR(m_logService)("ICUUnicodeService::unicodeToUtf8 conv %S - err %s"
                , _unicode
                , u_errorName(err_code_conv)
                );

            return false;
        }

        if( _utf8Size != NULL )
        {
            *_utf8Size = result_len;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ICUUnicodeService::utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize )
    {
        int32_t unicode_len;

        UErrorCode err_code_len = U_ZERO_ERROR;
        u_strFromUTF8(NULL, 0, &unicode_len, _utf8, _utf8Size, &err_code_len );

        if( U_FAILURE(err_code_len) && (err_code_len != U_BUFFER_OVERFLOW_ERROR)) 
        {
            LOGGER_ERROR(m_logService)("ICUUnicodeService::utf8ToUnicode size %s - err %s"
                , _utf8
                , u_errorName(err_code_len)
                );

            return false;
        }

        *_unicodeSize = unicode_len;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ICUUnicodeService::utf8ToUnicode( const char * _utf8, size_t _utf8Size, wchar_t * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{
		UErrorCode err_code_conv = U_ZERO_ERROR;

		int32_t result_len;
		u_strFromUTF8( _unicode, _unicodeCapacity, &result_len, _utf8, _utf8Size, &err_code_conv );

		if( U_FAILURE(err_code_conv) ) 
		{
			LOGGER_ERROR(m_logService)("ICUUnicodeService::utf8ToUnicode conv %s - err %s"
				, _utf8
				, u_errorName(err_code_conv)
				);
			
			return false;
		}

        if( _sizeUnicode != NULL )
        {
		    *_sizeUnicode = result_len;
        }

		return true;
	}
}