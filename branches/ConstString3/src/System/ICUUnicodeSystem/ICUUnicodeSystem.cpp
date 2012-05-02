#	include "ICUUnicodeSystem.h"

#	include "unicode/ustring.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::UnicodeInterface **_system )
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
void releaseInterfaceSystem( Menge::UnicodeInterface *_system )
{
	delete static_cast<Menge::ICUUnicodeService*>( _system );
}
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	String ICUUnicodeService::unicodeToUtf8( const WString& _unicode, bool & _successful )
	{
		int32_t utf8_len;

		UErrorCode err_code_len;
		u_strToUTF8(NULL, 0, &utf8_len, _unicode.c_str(), _unicode.size(), &err_code_len );

		String utf8;

		if( U_FAILURE(err_code_len) && (err_code_len != U_BUFFER_OVERFLOW_ERROR)) 
		{
			_successful = false;

			return utf8;
		}

		utf8.resize( utf8_len );

		UErrorCode err_code_conv;

		int32_t result_len;
		u_strToUTF8( &utf8[0], utf8_len, &result_len, _unicode.c_str(), _unicode.size(), &err_code_conv );

		if( U_FAILURE(err_code_conv) ) 
		{
			_successful = false;

			return utf8;
		}

		_successful = true;

		return utf8;
	}
	//////////////////////////////////////////////////////////////////////////
	WString ICUUnicodeService::utf8ToUnicode( const String& _utf8, bool & _successful )
	{
		int32_t unicode_len;

		size_t utf8_size = _utf8.size();

		UErrorCode err_code_len;
		u_strFromUTF8(NULL, 0, &unicode_len, _utf8.c_str(), utf8_size, &err_code_len );

		WString unicode;

		if( U_FAILURE(err_code_len) && (err_code_len != U_BUFFER_OVERFLOW_ERROR)) 
		{
			_successful = false;

			return unicode;
		}

		unicode.resize( unicode_len );

		UErrorCode err_code_conv;

		int32_t result_len;
		u_strFromUTF8( &unicode[0], unicode_len, &result_len, _utf8.c_str(), _utf8.size(), &err_code_conv );

		if( U_FAILURE(err_code_conv) ) 
		{
			_successful = false;

			return unicode;
		}

		_successful = true;

		return unicode;
	}
}