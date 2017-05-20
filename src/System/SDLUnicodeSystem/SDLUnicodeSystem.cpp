// Sergii Kudlai - well, this intended to be using SDL_iconv
//                 but then I realized that we can use C++11
//                 Should I rename this system now?

#   include "SDLUnicodeSystem.h"

#   include "Logger/Logger.h"

#   include <locale>
#   include <codecvt>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::SDLUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLUnicodeSystem::SDLUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
		try
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
			std::string dest = (_unicodeSize == UNICODE_UNSIZE) ? convert.to_bytes( _unicode ) : convert.to_bytes( _unicode, _unicode + _unicodeSize );
			size_t utf8_size = dest.size();

			if( _utf8 != nullptr && _utf8Capacity < utf8_size )
			{
				return false;
			}

			if( _utf8 != nullptr )
			{
				std::copy( dest.begin(), dest.end(), _utf8 );
			}

			if( _utf8Size != nullptr )
			{
				*_utf8Size = utf8_size;
			}

			if( _utf8 != nullptr && _utf8Capacity != 0 )
			{
				_utf8[utf8_size] = '\0';
			}
		}
		catch( const std::range_error & _ex )
		{
			LOGGER_ERROR( m_serviceProvider )("SDLUnicodeSystem::unicodeToUtf8 catch std::range_error '%s'"
				, _ex.what()
				);

			return false;
		}


        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
    {
		try
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
			std::wstring dest = (_utf8Size == UNICODE_UNSIZE) ? convert.from_bytes( _utf8 ) : convert.from_bytes( _utf8, _utf8 + _utf8Size );
			size_t wc_size = dest.size();

			if( _unicode != nullptr &&_unicodeCapacity < wc_size )
			{
				return false;
			}

			if( _unicode != nullptr )
			{
				std::copy( dest.begin(), dest.end(), _unicode );
			}

			if( _sizeUnicode != nullptr )
			{
				*_sizeUnicode = wc_size;
			}

			if( _unicode != nullptr && _unicodeCapacity != 0 )
			{
				_unicode[wc_size] = L'\0';
			}
		}
		catch( const std::range_error & _ex )
		{
			LOGGER_ERROR( m_serviceProvider )("SDLUnicodeSystem::utf8ToUnicode catch std::range_error '%s'"
				, _ex.what()
				);

			return false;
		}

        return true;
    }
}
